/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "thunk.hh"

#include <unistd.h>
#include <cstring>
#include <iostream>

#include "system_runner.hh"
#include "thunk_writer.hh"

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

int Thunk::execute( const roost::path & root_dir, const roost::path & thunk_path ) const
{
  if ( order_ != 1 ) {
    throw runtime_error( "cannot execute thunk with order != 1" );
  }

  // preparing argv
  vector<string> args = function_.args();
  args.insert( args.begin(), function_.exe() );

  // preparing envp
  vector<string> envars = {
    "PATH=/__gg__/bin/.",
    "__GG_THUNK_PATH__=" + thunk_path.string(),
    "__GG_DIR__=" + root_dir.string(),
    "__GG_ENABLED__=1",
    // "__GG_VERBOSE__=1"
  };

  int retval;

  cerr << "+ " << command_str( args, envars ) << endl;

  if ( ( retval = ezexec( ( root_dir / function_.hash() ).string(), args, envars ) ) < 0 ) {
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

void put_file( const roost::path & src, const roost::path & dst )
{
  if ( roost::exists( dst ) ) {
    /* XXX we might want to implement strict checks, like hash check */
    return;
  }

  roost::copy_file( src, dst );
}

void Thunk::collect_infiles( const roost::path & gg_dir ) const
{
  for ( InFile infile : infiles_ ) {
    if ( infile.hash().length() == 0 ) {
      /* this is a directory, not a file. no need to copy anything */
      continue;
    }

    roost::path source_path = infile.real_filename();
    roost::path target_path = gg_dir / infile.hash();
    put_file( source_path, target_path );
  }
}

void Thunk::store( const roost::path & gg_dir ) const
{
  collect_infiles( gg_dir );

  ThunkWriter::write_thunk( *this );
  string thunk_hash = InFile::compute_hash( outfile() );
  roost::copy_file( outfile(), gg_dir / thunk_hash );
}

bool Thunk::operator==( const Thunk & other ) const
{
  return ( outfile_ == other.outfile_ ) and
         ( function_ == other.function_ ) and
         ( infiles_ == other.infiles_ ) and
         ( order_ == other.order_ );
}
