/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <unistd.h>
#include <cstring>
#include <getopt.h>
#include <iostream>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <deque>
#include <list>
#include <vector>

#include "exception.hh"
#include "thunk.hh"
#include "thunk_reader.hh"
#include "child_process.hh"
#include "sandbox.hh"
#include "path.hh"
#include "temp_file.hh"
#include "temp_dir.hh"
#include "thunk_writer.hh"
#include "ggpaths.hh"
#include "placeholder.hh"
#include "system_runner.hh"
#include "graph.hh"
#include "poller.hh"
#include "signalfd.hh"
#include "util.hh"
#include "s3.hh"
#include "digest.hh"
#include "remote.hh"
#include "secure_socket.hh"

using namespace std;
using namespace gg::thunk;
using namespace PollerShortNames;

using ReductionResult = gg::cache::ReductionResult;

const bool sandboxed = ( getenv( "GG_SANDBOXED" ) != NULL );
const bool remote_execution = ( getenv( "GG_REMOTE" ) != NULL );

class Reductor
{
private:
  const string thunk_hash_;
  size_t max_jobs_;

  SignalMask signals_ { SIGCHLD, SIGCONT, SIGHUP, SIGTERM, SIGQUIT, SIGINT };
  Poller poller_ {};

  unordered_set<std::string> remote_executions_ {};
  list<ChildProcess> child_processes_ {};

  deque<string> dep_queue_ {};
  DependencyGraph dep_graph_ {};

  lambda::RequestGenerator request_generator_ { {}, gg::remote::s3_region() };
  lambda::ExecutionConnectionManager connection_manager_ { gg::remote::s3_region() };

  void execution_finalize( const string & old_hash, const string & new_hash );
  Result handle_signal( const signalfd_siginfo & sig );

  bool is_finished() const;

public:
  Reductor( const string & thunk_hash, const size_t max_jobs = 8 );

  string reduce();
  void upload_dependencies() const;
};

Reductor::Reductor( const string & thunk_hash, const size_t max_jobs )
  : thunk_hash_( thunk_hash ), max_jobs_( max_jobs )
{
  signals_.set_as_mask();

  dep_graph_.add_thunk( thunk_hash_ );
  unordered_set<string> o1_deps = dep_graph_.order_one_dependencies( thunk_hash_ );
  dep_queue_.insert( dep_queue_.end(), o1_deps.begin(), o1_deps.end() );
}

bool Reductor::is_finished() const
{
  return ( remote_executions_.size() == 0 ) and
         ( child_processes_.size() == 0 ) and
         ( dep_queue_.size() == 0 );
}

void Reductor::execution_finalize( const string & old_hash, const string & new_hash )
{
  unordered_set<string> new_o1s = dep_graph_.force_thunk( old_hash, new_hash );
  dep_queue_.insert( dep_queue_.end(), new_o1s.begin(), new_o1s.end() );
}

Result Reductor::handle_signal( const signalfd_siginfo & sig )
{
  switch ( sig.ssi_signo ) {
  case SIGCONT:
    for ( auto & child : child_processes_ ) {
      child.resume();
    }
    break;

  case SIGCHLD:
    if ( child_processes_.empty() ) {
      throw runtime_error( "received SIGCHLD without any managed children" );
    }

    for ( auto it = child_processes_.begin(); it != child_processes_.end(); it++ ) {
      ChildProcess & child = *it;

      if ( child.terminated() or ( not child.waitable() ) ) {
        continue;
      }

      child.wait( true );

      if ( child.terminated() ) {
        if ( child.exit_status() != 0 ) {
          child.throw_exception();
        }

        /* Update the dependency graph now that we know this process ended
        with exit code 0. */
        const string & thunk_hash = child.name();
        Optional<ReductionResult> result = gg::cache::check( thunk_hash );

        if ( not result.initialized() or result->order != 0 ) {
          throw runtime_error( "could not find the reduction entry" );
        }

        execution_finalize( thunk_hash, result->hash );

        it = child_processes_.erase( it );
        it--;

        if ( is_finished() ) {
          return ResultType::Exit;
        }
      }
      else if ( not child.running() ) {
        /* suspend parent too */
        CheckSystemCall( "raise", raise( SIGSTOP ) );
      }
    }

    break;

  case SIGHUP:
  case SIGTERM:
  case SIGQUIT:
  case SIGINT:
    throw runtime_error( "interrupted by signal" );

  default:
    throw runtime_error( "unknown signal" );
  }

  return ResultType::Continue;
}

string Reductor::reduce()
{
  SignalFD signal_fd( signals_ );

  poller_.add_action(
    Poller::Action(
      signal_fd.fd(), Direction::In,
      [&]() { return handle_signal( signal_fd.read_signal() ); },
      [&]() { return not child_processes_.empty(); }
    )
  );

  const bool remote_exec = remote_execution;

  while ( true ) {
    while ( not dep_queue_.empty() and child_processes_.size() < max_jobs_ ) {
      const string & dependency_hash = dep_queue_.front();

      /* don't bother executing gg-execute if it's in the cache */
      Optional<ReductionResult> cache_entry = gg::cache::check( dependency_hash );
      if ( cache_entry.initialized() ) {
        unordered_set<string> new_o1s = dep_graph_.force_thunk( dependency_hash, cache_entry->hash );
        dep_queue_.insert( dep_queue_.end(), new_o1s.begin(), new_o1s.end() );
      }
      else {
        if ( not remote_exec ) {
          /* for local execution, we just fork and run gg-execute. */
          child_processes_.emplace_back(
            dependency_hash,
            [dependency_hash]()
            {
              vector<string> command { "gg-execute", dependency_hash };
              return ezexec( command[ 0 ], command, {}, true, true );
            }
          );
        }
        else {
          remote_executions_.insert( dependency_hash );

          /* create new socket */
          SecureSocket & socket = connection_manager_.new_connection( dependency_hash );

          poller_.add_action(
            Poller::Action(
              socket, Direction::Out,
              [dependency_hash, &socket, this]()
              {
                HTTPRequest request = request_generator_.generate( dep_graph_.get_thunk( dependency_hash ),
                                                                   dependency_hash );
                connection_manager_.response_parser( dependency_hash ).new_request_arrived( request );
                socket.write( request.str() );

                return ResultType::Cancel;
              },
              [&]() { return not remote_executions_.empty(); }
            )
          );

          poller_.add_action(
            Poller::Action(
              socket, Direction::In,
              [dependency_hash, &socket, this]()
              {
                auto & response_parser = connection_manager_.response_parser( dependency_hash );
                response_parser.parse( socket.read() );

                if ( not response_parser.empty() ) {
                  RemoteResponse response { response_parser.front().body() };

                  if ( response.thunk_hash != dependency_hash ) {
                    throw runtime_error( "unexpected hash" );
                  }

                  gg::cache::insert( response.thunk_hash, response.output_hash );
                  cerr << "\u03bb(" + response.thunk_hash.substr( 0, 12 ) + ") = " +
                          response.output_hash.substr( 0, 12 ) << endl;

                  execution_finalize( response.thunk_hash, response.output_hash );

                  remote_executions_.erase( response.thunk_hash );
                  connection_manager_.remove_connection( response.thunk_hash );

                  if ( is_finished() ) {
                    return ResultType::Exit;
                  }
                  else {
                    return ResultType::Cancel;
                  }
                }

                return ResultType::Continue;
              },
              [&]() { return not remote_executions_.empty(); }
            )
          );
        }
      }

      dep_queue_.pop_front();
    }

    const auto poll_result = poller_.poll( -1 );

    if ( poll_result.result == Poller::Result::Type::Exit ) {
      const string final_hash = dep_graph_.updated_hash( thunk_hash_ );
      const Optional<ReductionResult> answer = gg::cache::check( final_hash );
      if ( not answer.initialized() ) {
        throw runtime_error( "internal error: final answer not found" );
      }
      return answer->hash;
    }
  }
}

void Reductor::upload_dependencies() const
{
  vector<S3::UploadRequest> upload_requests;

  for ( const string & dep : dep_graph_.order_zero_dependencies() ) {
    if ( gg::remote::is_available( dep ) ) {
      continue;
    }

    upload_requests.push_back( { gg::paths::blob_path( dep ), dep,
                                 digest::gghash_to_hex( dep ) } );
  }

  if ( upload_requests.size() == 0 ) {
    cerr << "No files to upload." << endl;
    return;
  }

  const string plural = upload_requests.size() == 1 ? "" : "s";
  cerr << "Uploading " << upload_requests.size() << " file" << plural << "... ";

  S3ClientConfig s3_config;
  s3_config.region = gg::remote::s3_region();

  S3Client s3_client;
  s3_client.upload_files(
    gg::remote::s3_bucket(), upload_requests,
    [] ( const S3::UploadRequest & upload_request )
    { gg::remote::set_available( upload_request.object_key ); }
  );

  cerr << "done." << endl;
}

void usage( const char * argv0 )
{
  cerr << argv0 << " THUNK [execution args]" << endl
       << endl
       << "Useful environment variables:" << endl
       << "  GG_DIR       => absolute path to gg directory" << endl
       << "  GG_SANDBOXED => if set, forces the thunks in a sandbox" << endl
       << "  GG_MAXJOBS   => maximum number of jobs to run in parallel" << endl
       << "  GG_REMOTE    => execute the thunks on AWS Lambda" << endl
       << endl;
}

int main( int argc, char * argv[] )
{
  try {
    if ( argc <= 0 ) {
      abort();
    }

    if ( argc < 2 ) {
      usage( argv[ 0 ] );
      return EXIT_FAILURE;
    }

    gg::models::init();

    size_t max_jobs = sysconf( _SC_NPROCESSORS_ONLN );
    string thunk_filename { argv[ 1 ] };
    const roost::path thunk_path = roost::canonical( thunk_filename );

    if ( getenv( "GG_MAXJOBS" ) != nullptr ) {
      max_jobs = stoul( safe_getenv( "GG_MAXJOBS" ) );
    }

    string thunk_hash;

    /* first check if this file is actually a placeholder */
    Optional<ThunkPlaceholder> placeholder = ThunkPlaceholder::read( thunk_path.string() );

    if ( not placeholder.initialized() ) {
      ThunkReader thunk_reader { thunk_path.string() };

      if( not thunk_reader.is_thunk() ) {
        /* not a placeholder and not a thunk. Our work is done here. */
        return EXIT_SUCCESS;
      }
      else {
        thunk_hash = InFile::compute_hash( thunk_path.string() );
      }
    }
    else {
      thunk_hash = placeholder->content_hash();
    }

    Reductor reductor { thunk_hash, max_jobs };

    if ( remote_execution ) {
      reductor.upload_dependencies();
    }

    string reduced_hash = reductor.reduce();

    if ( remote_execution ) {
      /* we need to fetch the output from S3 */
      S3ClientConfig s3_config;
      s3_config.region = gg::remote::s3_region();

      S3Client s3_client;
      s3_client.download_file(
        gg::remote::s3_bucket(), reduced_hash, gg::paths::blob_path( reduced_hash )
      );
    }

    roost::copy_then_rename( gg::paths::blob_path( reduced_hash ), thunk_path );

    return EXIT_SUCCESS;
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }
}
