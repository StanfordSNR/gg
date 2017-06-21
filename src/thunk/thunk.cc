/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "thunk.hh"

#include <unistd.h>
#include <cstring>
#include <iostream>

using namespace std;
using namespace gg;
using namespace gg::thunk;

Thunk::Thunk( const string & outfile, const Function & function,
              const vector<InFile> & infiles )
  : outfile_( outfile ), function_( function ), infiles_( infiles ),
    order_( compute_order() )
{}

Thunk::Thunk( const gg::protobuf::Thunk & thunk_proto )
  : outfile_( thunk_proto.outfile() ), function_( thunk_proto.function() ),
    infiles_(), order_()
{
  for ( protobuf::InFile infile : thunk_proto.infiles() ) {
    infiles_.push_back( { infile } );
  }

  order_ = compute_order();
}

int Thunk::execute( const string & root_dir ) const
{
  if ( order_ != 1 ) {
    throw runtime_error( "cannot execute thunk with order != 1" );
  }

  size_t i = 0;

  // preparing argv
  vector<string> args = function_.args();
  char ** argv = new char * [ args.size() + 1 /* NULL at the end */ ];

  args[ 0 ] = root_dir + function_.hash();

  for ( i = 0; i < args.size(); i++ ) {
    argv[ i ] = new char[ args[ i ].length() + 1 ];
    args[ i ].copy( argv[ i ], args[ i ].length() );
    argv[ i ][ args[ i ].length() ] = '\0';
  }

  argv[ args.size() ] = NULL;

  // preparing envp
  vector<string> gg_envars = {
    "PATH=" + root_dir + "exe/bin",
    "GG=1",
    //"GG_VERBOSE=1"
  };

  size_t envp_len = infiles_.size() + gg_envars.size();
  char ** envp = new char * [ envp_len + 1 ];

  for ( i = 0; i < infiles_.size(); i++ ) {
    string envar = infiles_[ i ].to_envar( root_dir );
    envp[ i ] = new char[ envar.length() + 1 ];
    envar.copy( envp[ i ], envar.length() );
    envp[ i ][ envar.length() ] = '\0';
  }

  for ( const string & gg_envar : gg_envars ) {
    envp[ i ] = new char[ gg_envar.length() + 1 ];
    strcpy( envp[ i++ ], gg_envar.c_str() );
  }

  envp[ i++ ] = NULL;

  int retval;

  if ( ( retval = execvpe( argv[ 0 ], argv, envp ) ) < 0 ) {
    for ( size_t i = 0; i < args.size(); i++ ) {
      delete[] argv[ i ];
    }

    for ( size_t i = 0; i < envp_len; i++ ) {
      delete[] envp[ i ];
    }

    delete[] argv;
    delete[] envp;

    throw runtime_error( "execvpe failed" );
  }

  return retval;
}

size_t Thunk::compute_order() const
{
  size_t order = 0;

  for ( const InFile & infile : infiles_ ) {
    order = max( infile.order(), order );
  }

  return order + 1;
}

protobuf::Thunk Thunk::to_protobuf() const
{
  protobuf::Thunk thunk;

  thunk.set_outfile( outfile_ );
  *thunk.mutable_function() = function_.to_protobuf();

  for ( const InFile & infile : infiles_ ) {
    *thunk.add_infiles() = infile.to_protobuf();
  }

  return thunk;
}

bool Thunk::operator==( const Thunk & other ) const
{
  return ( outfile_ == other.outfile_ ) and
         ( function_ == other.function_ ) and
         ( infiles_ == other.infiles_ ) and
         ( order_ == other.order_ );
}
