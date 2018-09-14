/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include <cstdlib>

#include "thunk/ggutils.hh"
#include "util/exception.hh"
#include "util/inotify.hh"
#include "util/poller.hh"
#include "util/path.hh"
#include "util/ipc_socket.hh"
#include "util/optional.hh"

using namespace std;
using namespace PollerShortNames;

bool ends_with( const string & str, const string & ending )
{
    return ( str.length() >= ending.length() )
           ? ( 0 == str.compare( str.length() - ending.length(), ending.length(), ending ) )
           : false;
}


void usage( const char * argv0 )
{
  cerr << argv0 << " DIR-NAME FILTERS SOCKET" << endl;
}

vector<string> read_filters( const string & filename )
{
  ifstream fin { filename };

  if ( not fin.good() ) {
    throw runtime_error( "filters file not found" );
  }

  vector<string> filters;
  string line;

  while ( getline( fin, line ) ) {
    filters.push_back( line );
  }

  return filters;
}

bool check_file( const roost::path & path, const vector<string> & filters )
{
  for ( const auto & filter : filters ) {
    if ( ends_with( path.string(), filter ) ) {
      return true;
    }
  }

  return false;
}

class Watcher
{
private:
  const roost::path root_;
  const vector<string> filters_;
  const string sock_path_;

  IPCSocket ipc_socket_ {};
  Poller poller_ {};
  Inotify notifier_ { poller_ };
  unordered_set<string> index_ {};
  Inotify::callback_t watch_callback_fn_;

  mutable Optional<string> index_str_cache_ { false };

  static constexpr int watch_flags { IN_MOVE | IN_DELETE | IN_CLOSE_WRITE | IN_CREATE |
  IN_ONLYDIR };

  void watch_callback( const inotify_event & event, const roost::path & root );
  void scan_directory( const roost::path & path );

public:
  Watcher( roost::path && root, vector<string> && filters, string && sock_path );
  Poller::Result loop_once() { return poller_.poll( -1 ); }

  const string & index_str() const;
};

Watcher::Watcher( roost::path && root, vector<string> && filters,
                  string && sock_path )
  : root_( move( root ) ), filters_( move( filters ) ), sock_path_( move( sock_path ) ),
    watch_callback_fn_( [this] ( const inotify_event & e, const roost::path & p ) {
                          this->watch_callback( e, p ); } )
{
  scan_directory( root_ );
  ipc_socket_.bind( sock_path_ );
  ipc_socket_.listen();
  cerr << "listening on " << "unix://" << sock_path_ << "..." << endl;

  poller_.add_action( { ipc_socket_, Direction::In,
    [this] () -> ResultType
    {
      FileDescriptor conn_fd = ipc_socket_.accept();
      const string & response = index_str();
      if ( response.length() ) {
        conn_fd.write( response );
      }
      return ResultType::Continue;
    } } );
}

const string & Watcher::index_str() const
{
  if ( index_str_cache_.initialized() ) {
    return *index_str_cache_;
  }

  ostringstream oss;
  for ( auto const & item : index_ ) {
    oss << item << endl;
  }

  index_str_cache_.reset( oss.str() );
  return *index_str_cache_;
}

void Watcher::watch_callback( const inotify_event & event, const roost::path & root )
{
  const string full_path = ( root / event.name ).string();

  if ( event.mask & IN_ISDIR ) {
    if ( event.mask & IN_CREATE ) {
      scan_directory( full_path );
    }
    else if ( event.mask & IN_DELETE ) {}
    else {
      throw runtime_error( "not handled" );
    }
  }
  else {
    if ( ( event.mask & IN_CLOSE_WRITE ) or ( event.mask & IN_MOVED_TO ) ) {
      if ( check_file( full_path, filters_ ) ) {
        index_str_cache_.clear();
        index_.emplace( full_path );
      }
    }
    else if ( ( event.mask & IN_MOVED_FROM ) or ( event.mask & IN_DELETE ) ) {
      index_.erase( full_path );
    }
    else if ( event.mask & IN_CREATE ) { /* just ignore */ }
    else {
      throw runtime_error( "not handled" );
    }
  }
}

void Watcher::scan_directory( const roost::path & root )
{
  notifier_.add_watch( root, watch_flags, watch_callback_fn_ );

  for ( const auto & entry : roost::get_directory_listing( root ) ) {
    const roost::path entry_path = root / entry;
    /* XXX */
    if ( roost::exists_and_is_directory( entry_path ) ) {
      scan_directory( entry_path );
    }
    else {
      if ( check_file( entry_path, filters_ ) ) {
        index_str_cache_.clear();
        index_.emplace( entry_path.string() );
      }
    }
  }
}

int main( int argc, char * argv[] )
{
  try {
    if ( argc <= 0 ) {
      abort();
    }

    if ( argc != 4 ) {
      usage( argv[ 0 ] );
      return EXIT_FAILURE;
    }

    Watcher watcher { roost::canonical( argv[ 1 ] ), read_filters( argv[ 2 ] ),
                      argv[ 3 ] };

    while ( true )  {
      auto poll_result = watcher.loop_once().result;
      if ( poll_result == Poller::Result::Type::Exit ) {
        break;
      }
    }
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
