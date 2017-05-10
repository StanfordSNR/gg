/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "thunk.hh"

#include <unistd.h>
#include <cstring>

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
    infiles_(), order_( compute_order() )
{
  for ( protobuf::InFile infile : thunk_proto.infiles() ) {
    infiles_.push_back( { infile } );
  }
}

int Thunk::execute() const
{
  if ( order_ != 1 ) {
    throw runtime_error( "cannot execute thunk with order != 1" );
  }

  // preparing argv

  const vector<string> args = function_.args();
  char ** argv = new char * [ args.size() + 1 ];

  for ( size_t i = 0; i < args.size(); i++ ) {
    argv[ i ] = new char[ args[ i ].length() + 1 ];
    args[ i ].copy( argv[ i ], args[ i ].length() );
    argv[ i ][ args[ i ].length() ] = '\0';
  }

  argv[ args.size() ] = NULL;

  // preparing envp
  char ** envp = new char * [ infiles_.size() + 1 ];

  for ( size_t i = 0; i < infiles_.size(); i++ ) {
    string envar = infiles_[ i ].to_envar();
    envp[ i ] = new char[ envar.length() + 1 ];
    envar.copy( envp[ i ], envar.length() );
    envp[ i ][ envar.length() ] = '\0';
  }

  envp[ infiles_.size() ] = NULL;

  int retval;

  if ( ( retval = execvpe( function_.exe().c_str(), argv, envp ) ) < 0 ) {
    for ( size_t i = 0; i < args.size(); i++ ) {
      delete[] argv[ i ];
    }

    for ( size_t i = 0; i < infiles_.size(); i++ ) {
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
