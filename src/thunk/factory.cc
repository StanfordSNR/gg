/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "factory.hh"

#include <algorithm>
#include <chrono>
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
using namespace std::chrono;
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
    hash_ = gg::hash::file( real_filename_, { true, type_ } );
  }
}

Thunk ThunkFactory::create_thunk( const Function & function,
                                  const vector<Data> & data,
                                  const vector<Data> & executables,
                                  const vector<Output> & outputs,
                                  const milliseconds & timeout,
                                  const bool include_filenames )
{
  vector<Thunk::DataItem> thunk_data;
  vector<Thunk::DataItem> thunk_executables;
  vector<string> thunk_outputs;

  thunk::Function thunk_function { function };

  for ( const Data & datum : data ) {
    thunk_data.emplace_back( datum.hash(), include_filenames ? datum.filename()
                                                             : string {} );
  }

  for ( const Data & datum : executables ) {
    thunk_executables.emplace_back( datum.hash(), include_filenames ? datum.filename()
                                                                    : string {} );
  }

  for ( const Output & output : outputs ) {
    thunk_outputs.push_back( output.tag() );
  }

  Thunk output_thunk { move( thunk_function ), move( thunk_data ),
                       move( thunk_executables ), move( thunk_outputs ) };

  output_thunk.set_timeout( timeout );
  return output_thunk;
}

string ThunkFactory::generate( const Function & function,
                               const vector<Data> & data,
                               const vector<Data> & executables,
                               const vector<Output> & outputs,
                               const vector<string> & dummy_dirs,
                               const milliseconds & timeout, const int options )
{
  const bool generate_manifest = options & Options::generate_manifest;
  const bool create_placeholder = options & Options::create_placeholder;
  const bool collect_data = options & Options::collect_data;
  const bool include_filenames = options & Options::include_filenames;

  vector<Thunk::DataItem> thunk_data;
  vector<Thunk::DataItem> thunk_executables;
  vector<string> thunk_outputs;

  thunk::Function thunk_function { function };

  for ( const Data & datum : data ) {
    thunk_data.emplace_back( datum.hash(), include_filenames ? datum.filename()
                                                             : string {} );
  }

  for ( const Data & datum : executables ) {
    thunk_executables.emplace_back( datum.hash(), include_filenames ? datum.filename()
                                                                    : string {} );
  }

  for ( const Output & output : outputs ) {
    thunk_outputs.push_back( output.tag() );
  }

  if ( generate_manifest ) {
    FileManifest manifest;

    for ( const string & dir : dummy_dirs ) {
      manifest.add_dummy_directory( roost::path( dir ).lexically_normal().string() );
    }

    for ( const Output & output : outputs ) {
      if ( output.filename().initialized() ) {
        manifest.add_output_tag( *output.filename(), output.tag() );
      }
    }

    string manifest_data = manifest.serialize();
    string manifest_hash = gg::hash::compute( manifest_data,
                                              ObjectType::Value );
    thunk_data.emplace_back( manifest_hash, string {} );
    roost::atomic_create( manifest_data,
                          gg::paths::blob( manifest_hash ), true, 0400 );
    thunk_function.envars().push_back( "GG_MANIFEST=" + thunk::data_placeholder( manifest_hash ) );
  }

  if ( collect_data ) {
    auto fn_collect =
      [] ( const Data & datum, const bool executable )
      {
        if ( datum.real_filename().length() == 0 ) {
          return;
        }

        roost::path source_path = datum.real_filename();
        roost::path target_path = gg::paths::blob( gg::hash::base( datum.hash() ) );

        if ( not roost::exists( target_path ) ) {
          roost::copy_then_rename( source_path, target_path, true, executable ? 0500 : 0400 );
        }
      };

    for ( const Data & datum : data ) { fn_collect( datum, false ); }
    for ( const Data & datum : executables ) { fn_collect( datum, true ); }
  }

  Thunk output_thunk { move( thunk_function ), move( thunk_data ),
                       move( thunk_executables ), move( thunk_outputs ) };

  output_thunk.set_timeout( timeout );
  const string hash = ThunkWriter::write( output_thunk );

  if ( create_placeholder ) {
    for ( size_t i = 0; i < outputs.size(); i++ ) {
      const Output & output = outputs.at( i );

      const string output_hash = ( i == 0 ) ? hash : gg::hash::for_output( hash, output.tag() );
      ThunkPlaceholder placeholder { output_hash };

      if ( output.filename().initialized() ) {
        placeholder.write( *output.filename() );
      }
      else {
        placeholder.write( output.tag() );
      }
    }
  }

  return hash;
}
