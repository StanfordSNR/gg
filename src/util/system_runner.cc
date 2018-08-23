/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sstream>
#include <array>

#include "system_runner.hh"
#include "child_process.hh"
#include "exception.hh"
#include "file_descriptor.hh"
#include "pipe.hh"
#include "optional.hh"

using namespace std;

int ezexec( const string & filename, const vector<string> & args,
            const vector<string> & env, const bool use_environ,
            const bool path_search )
{
  if ( args.empty() ) {
    throw runtime_error( "ezexec: empty args" );
  }

  /* XXX revisit this
  if ( geteuid() == 0 or getegid() == 0 ) {
    if ( environ ) {
      throw runtime_error( "BUG: root's env not cleared" );
    }

    if ( path_search ) {
      throw runtime_error( "BUG: root should not search PATH" );
    }
  } */

  /* copy the arguments to mutable structures */
  vector<char *> argv;
  vector<vector<char>> argv_data;

  for ( auto & x : args ) {
    vector<char> new_str;
    for ( auto & ch : x ) {
      new_str.push_back( ch );
    }
    new_str.push_back( 0 ); /* null-terminate */

    argv_data.push_back( new_str );
  }

  for ( auto & x : argv_data ) {
    argv.push_back( &x[ 0 ] );
  }

  argv.push_back( 0 ); /* null-terminate */

  /* copy the env variables to mutable structures */
  vector<char *> envp;
  vector<vector<char>> envp_data;

  if ( not use_environ ) {
    for ( auto & x : env ) {
      vector<char> new_str;
      for ( auto & ch : x ) {
        new_str.push_back( ch );
      }
      new_str.push_back( 0 ); /* null-terminate */

      envp_data.push_back( new_str );
    }

    for ( auto & x : envp_data ) {
      envp.push_back( &x[ 0 ] );
    }

    envp.push_back( 0 ); /* null-terminate */
  }

  return ( path_search ? execvpe : execve )( filename.c_str(), &argv[ 0 ],
                                             use_environ ? environ : &envp[ 0 ] );
}

string run( const string & filename, const vector<string> & args,
            const vector<string> & env, const bool use_environ,
            const bool path_search, const bool read_stdout_until_eof,
            const bool suppress_errors )
{
  string output;

  Optional<pair<FileDescriptor, FileDescriptor>> pipe { read_stdout_until_eof, make_pipe() };

  ChildProcess command_process( args[ 0 ],
    [&]()
    {
      if ( read_stdout_until_eof ) {
        CheckSystemCall( "dup2", dup2( pipe->second.fd_num(), STDOUT_FILENO ) );
      }

      if ( suppress_errors ) {
        FileDescriptor devnull { CheckSystemCall( "open /dev/null",
                                                  open( "/dev/null", O_RDONLY ) ) };
        CheckSystemCall( "dup2", dup2( devnull.fd_num(), STDERR_FILENO ) );
      }

      return ezexec( filename, args, env, use_environ, path_search );
    }
  );

  if ( read_stdout_until_eof ) {
    pipe->second.close();
    while ( not pipe->first.eof() ) {
      output.append( pipe->first.read() );
    }
  }

  while ( !command_process.terminated() ) {
    command_process.wait();
  }

  if ( (not suppress_errors) and (command_process.exit_status() != 0) ) {
    command_process.throw_exception();
  }

  return output;
}

string command_str( const vector<string> & command,
                    const vector<string> & environment )
{
  ostringstream oss;

  for ( const auto & e : environment ) { oss << e << " "; }
  for ( const auto & c : command ) { oss << c << " "; }

  return oss.str();
}

std::string command_str( const int argc, char * argv[] )
{
    ostringstream oss;
    for ( int i = 0; i < argc; i++ ) {
        oss << argv[ i ];

        if ( i != argc - 1 )  {
            oss << " ";
        }
    }

    return oss.str();
}
