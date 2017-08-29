/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "thunk.hh"

#include <unistd.h>
#include <cstring>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <numeric>

#include "system_runner.hh"
#include "thunk_writer.hh"
#include "temp_file.hh"
#include "placeholder.hh"
#include "digest.hh"
#include "ggpaths.hh"

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

int Thunk::execute( const string & thunk_hash ) const
{
  if ( order_ != 1 ) {
    throw runtime_error( "cannot execute thunk with order != 1" );
  }

  // preparing argv
  vector<string> args = function_.args();

  /* do we need to replace a filename with its hash? */
  for ( string & arg : args ) {
    const size_t replace_begin = arg.find( BEGIN_REPLACE );
    if ( replace_begin == string::npos ) {
      continue;
    }

    const size_t filename_begin = replace_begin + BEGIN_REPLACE.length();

    const size_t replace_end = arg.find( END_REPLACE, filename_begin );
    if ( replace_end == string::npos ) {
      throw runtime_error( "invalid GG argument replacement: " + arg );
    }

    const string filename = arg.substr( filename_begin,
                                        replace_end - filename_begin );

    const string replacement = filename_to_hash( filename );

    arg.replace( replace_begin,
                 BEGIN_REPLACE.length() + filename.length() + END_REPLACE.length(),
                 replacement );
  }

  args.insert( args.begin(), function_.exe() );

  const roost::path thunk_path = gg::paths::blob_path( thunk_hash );

  // preparing envp
  const vector<string> & f_envars = function_.envars();
  vector<string> envars = {
    "__GG_THUNK_PATH__=" + thunk_path.string(),
    "__GG_DIR__=" + gg::paths::blobs().string(),
    "__GG_ENABLED__=1",
  };

  if ( getenv( "__GG_VERBOSE__" ) ) {
    envars.emplace_back( "__GG_VERBOSE__=1" );
  }

  envars.insert( envars.end(), f_envars.begin(), f_envars.end() );

  int retval;

  cerr << "+ " << command_str( args, envars ) << endl;

  if ( ( retval = ezexec( gg::paths::blob_path( function_.hash() ).string(), args, envars ) ) < 0 ) {
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

void Thunk::collect_infiles() const
{
  for ( InFile infile : infiles_ ) {
    if ( infile.content_hash().length() == 0 ) {
      /* this is a directory, not a file. no need to copy anything */
      continue;
    }

    roost::path source_path = infile.real_filename();
    roost::path target_path = gg::paths::blob_path( infile.content_hash() );
    put_file( source_path, target_path );
  }
}

string Thunk::store() const
{
  collect_infiles();

  const string thunk_hash = ThunkWriter::write_thunk( *this );

  // create the placeholder
  ThunkPlaceholder placeholder { thunk_hash, order(),
                                 roost::file_size( paths::blob_path( thunk_hash ) ) };
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

string Thunk::executable_hash() const
{
  vector<string> executable_hashes;

  for ( const InFile & infile : infiles_ ) {
    if ( infile.type() == InFile::Type::EXECUTABLE ) {
      executable_hashes.push_back( infile.content_hash() );
    }
  }

  sort( executable_hashes.begin(), executable_hashes.end() );

  const string combined_hashes = accumulate( executable_hashes.begin(),
                                             executable_hashes.end(),
                                             string {} );

  return digest::sha256( combined_hashes );
}

void Thunk::update_infile( const string & old_hash, const string & new_hash,
                           const size_t new_order, const off_t new_size )
{
  for ( size_t i = 0; i < infiles_.size(); i++ ) {
    if ( infiles_[ i ].content_hash() == old_hash ) {
      InFile new_infile { infiles_[ i ].filename(), infiles_[ i ].real_filename(),
                          new_hash, new_order, new_size, infiles_[ i ].type() };

      infiles_[ i ] = new_infile;
      order_ = compute_order();
      return;
    }
  }

  throw runtime_error( "infile doesn't exist: " + old_hash );
}

string Thunk::filename_to_hash( const string & filename ) const
{
  for ( const InFile & infile : infiles_ ) {
    if ( infile.filename() == filename ) {
      return infile.content_hash();
    }
  }

  throw runtime_error( "filename " + filename + " not found in thunk" );
}

unordered_map<string, Permissions>
Thunk::get_allowed_files( const std::string & thunk_hash ) const
{
  unordered_map<string, Permissions> allowed_files;

  for ( const InFile & infile : infiles() ) {
    if ( infile.content_hash().length() ) {
      if ( infile.type() == InFile::Type::FILE ) {
        allowed_files[ gg::paths::blob_path( infile.content_hash() ).string() ] = { true, false, false };
      }
      else if ( infile.type() == InFile::Type::EXECUTABLE ) {
        allowed_files[ gg::paths::blob_path( infile.content_hash() ).string() ] = { true, false, true };
      }
    }
    else {
      allowed_files[ infile.filename() ] = { true, false, false };
    }
  }

  allowed_files[ gg::paths::blobs().string() ] = { true, false, false };
  allowed_files[ gg::paths::blob_path( thunk_hash ).string() ] = { true, false, false };
  allowed_files[ outfile() ] = { true, true, false };

  return allowed_files;
}
