/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "thunk.hh"

#include <unistd.h>
#include <cstring>
#include <iostream>
#include <unordered_map>

#include "system_runner.hh"
#include "thunk_writer.hh"
#include "temp_file.hh"
#include "placeholder.hh"

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
  const vector<string> & f_envars = function_.envars();
  vector<string> envars = {
    "__GG_THUNK_PATH__=" + thunk_path.string(),
    "__GG_DIR__=" + root_dir.string(),
    "__GG_ENABLED__=1",
    // "__GG_VERBOSE__=1"
  };

  envars.insert( envars.end(), f_envars.begin(), f_envars.end() );

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

  roost::copy_then_rename( src, dst );
}

void Thunk::collect_infiles( const roost::path & gg_dir ) const
{
  for ( InFile infile : infiles_ ) {
    if ( infile.content_hash().length() == 0 ) {
      /* this is a directory, not a file. no need to copy anything */
      continue;
    }

    roost::path source_path = infile.real_filename();
    roost::path target_path = gg_dir / infile.content_hash();
    put_file( source_path, target_path );
  }
}

string Thunk::store( const roost::path & gg_dir ) const
{
  collect_infiles( gg_dir );

  UniqueFile temp_thunk { ( gg_dir / "thunk" ).string() };

  ThunkWriter::write_thunk( *this, temp_thunk.name() );
  string thunk_hash = InFile::compute_hash( temp_thunk.name() );
  roost::path thunk_in_gg_path = gg_dir / thunk_hash;

  if ( not roost::exists( thunk_in_gg_path ) ) {
    roost::rename( temp_thunk.name(), thunk_in_gg_path );
  }
  else {
    roost::remove( temp_thunk.name() );
  }

  // create the placeholder
  ThunkPlaceholder placeholder { thunk_hash, order(), roost::file_size( thunk_in_gg_path ) };
  placeholder.write( outfile() );

  return thunk_hash;
}

bool Thunk::operator==( const Thunk & other ) const
{
  return ( outfile_ == other.outfile_ ) and
         ( function_ == other.function_ ) and
         ( infiles_ == other.infiles_ ) and
         ( order_ == other.order_ );
}

unordered_map<string, Permissions> Thunk::get_allowed_files( const roost::path & gg_path,
                                                             const roost::path & thunk_path ) const
{
  unordered_map<string, Permissions> allowed_files;

  for ( const InFile & infile : infiles() ) {
    if ( infile.content_hash().length() ) {
      if ( infile.type() == InFile::Type::FILE ) {
        allowed_files[ ( gg_path / infile.content_hash() ).string() ] = { true, false, false };
      }
      else if ( infile.type() == InFile::Type::EXECUTABLE ) {
        allowed_files[ ( gg_path / infile.content_hash() ).string() ] = { true, false, true };
      }
    }
    else {
      allowed_files[ infile.filename() ] = { true, false, false };
    }
  }

  allowed_files[ gg_path.string() ] = { true, false, false };
  allowed_files[ thunk_path.string() ] = { true, false, false };
  allowed_files[ outfile() ] = { true, true, false };

  return allowed_files;
}
