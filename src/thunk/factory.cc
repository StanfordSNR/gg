/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "factory.hh"

#include <algorithm>
#include <sys/stat.h>
#include <fcntl.h>

#include "ggutils.hh"
#include "manifest.hh"
#include "placeholder.hh"
#include "thunk.hh"
#include "thunk_writer.hh"
#include "util/exception.hh"
#include "util/file_descriptor.hh"
#include "util/optional.hh"
#include "util/path.hh"
#include "util/tokenize.hh"

using namespace std;
using namespace gg;
using namespace gg::thunk;

ThunkFactory::Data::Data( const string & filename,
                          const string & real_filename,
                          const ObjectType & type,
                          const string & hash )
  : filename_( roost::path( filename ).lexically_normal().string() ),
    real_filename_( ( real_filename.length() ) ? real_filename : filename_ ),
    hash_( hash ), type_( type )
{
  if ( hash_.length() ) {
    return;
  }

  Optional<ThunkPlaceholder> placeholder = ThunkPlaceholder::read( real_filename_ );

  if ( placeholder.initialized() ) {
    type_ = ObjectType::Thunk;
    hash_ = placeholder->content_hash();
  }
  else {
    type_ = type;
    hash_ = compute_hash( real_filename_, type_ );
  }
}

string ThunkFactory::Data::compute_hash( const string & real_filename,
                                         const gg::ObjectType type )
{
  /* do we have this hash in cache? */
  struct stat file_stat;
  CheckSystemCall( "stat", stat( real_filename.c_str(), &file_stat ) );

  const auto cache_entry_path = gg::paths::hash_cache_entry( real_filename, file_stat );

  if ( roost::exists( cache_entry_path ) ) {
    FileDescriptor cache_file { CheckSystemCall( "open",
                                                 open( cache_entry_path.string().c_str(), O_RDONLY ) ) };
    string cache_entry;
    while ( not cache_file.eof() ) { cache_entry += cache_file.read(); }

    vector<string> cache_contents = split( cache_entry, " " );

    if ( cache_contents.size() != 6 ) {
      throw runtime_error( "bad cache entry: " + cache_entry_path.string() );
    }

    if ( cache_contents.at( 0 ) == to_string( file_stat.st_size )
         and cache_contents.at( 1 ) == to_string( file_stat.st_mtim.tv_sec )
         and cache_contents.at( 2 ) == to_string( file_stat.st_mtim.tv_nsec )
         and cache_contents.at( 3 ) == to_string( file_stat.st_ctim.tv_sec )
         and cache_contents.at( 4 ) == to_string( file_stat.st_ctim.tv_nsec ) ) {
      /* cache hit! */
      return cache_contents.at( 5 );
    }
  }

  /* not a cache hit, so need to compute hash ourselves */

  FileDescriptor file { CheckSystemCall( "open (" + real_filename + ")",
                                         open( real_filename.c_str(), O_RDONLY ) ) };

  string contents;
  while ( not file.eof() ) { contents += file.read(); }

  const string computed_hash = gg::hash::compute( contents, type );

  /* make a cache entry */
  atomic_create( to_string( file_stat.st_size ) + " "
                 + to_string( file_stat.st_mtim.tv_sec ) + " "
                 + to_string( file_stat.st_mtim.tv_nsec ) + " "
                 + to_string( file_stat.st_ctim.tv_sec ) + " "
                 + to_string( file_stat.st_ctim.tv_nsec ) + " "
                 + computed_hash,
                 cache_entry_path );

  return computed_hash;
}

std::string ThunkFactory::generate( const Function & function,
                                    const std::vector<Data> & data,
                                    const std::vector<Data> & executables,
                                    const std::vector<Output> & outputs,
                                    const std::vector<std::string> & dummy_dirs,
                                    const int options )
{
  const bool generate_manifest = options & Options::generate_manifest;
  const bool create_placeholder = options & Options::create_placeholder;
  const bool collect_data = options & Options::collect_data;
  const bool include_filenames = options & Options::include_filenames;

  vector<Thunk::DataItem> thunk_data;
  vector<Thunk::DataItem> thunk_executables;
  vector<string> thunk_outputs;

  thunk::Function thunk_function { function };

  if ( generate_manifest ) {
    FileManifest manifest;

    for ( const Data & datum : data ) {
      thunk_data.emplace_back( datum.hash(),
                               include_filenames ? datum.filename()
                                                 : string {} );
    }

    for ( const Data & datum : executables ) {
      thunk_executables.emplace_back( datum.hash(),
                                      include_filenames ? datum.filename()
                                                        : string {} );
    }

    for ( const string & dir : dummy_dirs ) {
      manifest.add_dummy_directory( roost::path( dir ).lexically_normal().string() );
    }

    for ( const Output & output : outputs ) {
      thunk_outputs.push_back( output.tag() );

      if ( output.filename().initialized() ) {
        manifest.add_output_tag( *output.filename(), output.tag() );
      }
    }

    string manifest_data = manifest.serialize();
    string manifest_hash = gg::hash::compute( manifest_data,
                                              ObjectType::Value );
    thunk_data.emplace_back( manifest_hash, string {} );
    roost::atomic_create( manifest_data,
                          gg::paths::blob_path( manifest_hash ) );
    thunk_function.envars().push_back( "GG_MANIFEST=" + thunk::data_placeholder( manifest_hash ) );
  }

  if ( collect_data ) {
    auto fn_collect =
      [] ( const Data & datum )
      {
        roost::path source_path = datum.real_filename();
        roost::path target_path = gg::paths::blob_path( datum.hash() );

        if ( not roost::exists( target_path ) ) {
          roost::copy_then_rename( source_path, target_path );
        }
      };

    for ( const Data & datum : data ) { fn_collect( datum ); }
    for ( const Data & datum : executables ) { fn_collect( datum ); }
  }

  string hash = ThunkWriter::write_thunk( { move( thunk_function ),
                                            move( thunk_data ),
                                            move( thunk_executables ),
                                            move( thunk_outputs ) } );

  if ( create_placeholder and outputs.at( 0 ).filename().initialized() ) {
    ThunkPlaceholder placeholder { hash };
    placeholder.write( *outputs.at( 0 ).filename() );
  }

  return hash;
}
