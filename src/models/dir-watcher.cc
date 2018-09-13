/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <fstream>
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

using namespace std;

bool ends_with( const string & str, const string & ending )
{
    return ( str.length() >= ending.length() )
           ? ( 0 == str.compare( str.length() - ending.length(), ending.length(), ending ) )
           : false;
}


void usage( const char * argv0 )
{
  cerr << argv0 << " DIR-NAME FILTERS" << endl;
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

class Watcher
{
private:
  const roost::path root_;
  const vector<string> filters_;
  Poller poller_ {};
  Inotify notifier_ { poller_ };
  unordered_map<string, string> index_ {};
  Inotify::callback_t watch_callback_fn_;

  static constexpr int watch_flags { IN_MOVE | IN_DELETE | IN_CLOSE_WRITE | IN_CREATE |
  IN_ONLYDIR };

  void watch_callback( const inotify_event & event, const roost::path & root );
  void scan_directory( const roost::path & path );

public:
  Watcher( roost::path && root, vector<string> && filters );
  Poller::Result loop_once() { return poller_.poll( -1 ); }
};

Watcher::Watcher( roost::path && root, vector<string> && filters )
  : root_( move( root ) ), filters_( move( filters ) ),
    watch_callback_fn_( bind( &Watcher::watch_callback, this,
                              placeholders::_1, placeholders::_2 ) )
{
  scan_directory( root_ );
}

void Watcher::watch_callback( const inotify_event & event, const roost::path & root )
{
  const string full_path = ( root / event.name ).string();

  if ( event.mask & IN_ISDIR ) {
    if ( event.mask & IN_CREATE ) {
      notifier_.add_watch( full_path, watch_flags, watch_callback_fn_ );
    }
    else if ( event.mask & IN_DELETE ) {}
    else {
      throw runtime_error( "not handled" );
    }
  }
  else {
    if ( ( event.mask & IN_CLOSE_WRITE ) or ( event.mask & IN_MOVED_TO ) ) {
      index_.emplace( make_pair( full_path, gg::hash::file( full_path ) ) );
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
    if ( roost::is_directory( entry_path ) ) {
      scan_directory( entry_path );
    }
    else {
      for ( const auto & filter : filters_ ) {
        if ( ends_with( entry_path.string(), filter ) ) {
          index_.emplace( make_pair( entry_path.string(),
                                     gg::hash::file( entry_path ) ) );
          break;
        }
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

    if ( argc != 3 ) {
      usage( argv[ 0 ] );
      return EXIT_FAILURE;
    }

    Watcher watcher { roost::canonical( argv[ 1 ] ), read_filters( argv[ 2 ] ) };

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
