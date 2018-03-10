/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "thunk.hh"

#include <unistd.h>
#include <cstring>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <google/protobuf/util/json_util.h>

#include "thunk/ggutils.hh"
#include "thunk/placeholder.hh"
#include "thunk/thunk_writer.hh"
#include "util/digest.hh"
#include "util/system_runner.hh"
#include "util/temp_file.hh"

using namespace std;
using namespace gg;
using namespace gg::thunk;
using namespace CryptoPP;
using namespace google::protobuf::util;

template<class Iterator>
Thunk::Data::Data( Iterator begin, Iterator end )
{
  for ( auto it = begin; it != end; it++ ) {
    switch ( hash::type( *it ) ) {
    case ObjectType::Value: values.emplace( *it ); break;
    case ObjectType::Thunk: thunks.emplace( *it ); break;
    case ObjectType::Executable: executables.emplace( *it ); break;
    }
  }
}

Thunk::Data::Data( const vector<string> & data )
  : Data( data.cbegin(), data.cend() )
{}

bool Thunk::Data::operator==( const Data & other ) const
{
  return ( values == other.values ) and
         ( executables == other.executables ) and
         ( thunks == other.thunks );
}

Thunk::Thunk( const Function & function, const vector<string> & data,
              const vector<string> & outputs )
  : function_( function ), data_( data ), outputs_( outputs )
{}

Thunk::Thunk( const gg::protobuf::Thunk & thunk_proto )
  : function_( thunk_proto.function() ),
    data_( thunk_proto.data().cbegin(), thunk_proto.data().cend() ),
    outputs_( thunk_proto.outputs().cbegin(), thunk_proto.outputs().cend() )
{}

int Thunk::execute() const
{
  if ( data_.thunks.size() != 0 ) {
    throw runtime_error( "cannot execute thunk with unresolved dependencies" );
  }

  bool verbose = ( getenv( "GG_VERBOSE" ) != nullptr );

  // preparing argv
  vector<string> args = function_.args();

  /* do we need to replace a filename with its hash? */
  for ( string & arg : args ) {
    const size_t replace_begin = arg.find( BEGIN_REPLACE );

    if ( replace_begin != string::npos ) {
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
    else if ( arg.compare( 0, GG_HASH_REPLACE.length(), GG_HASH_REPLACE ) == 0 ) {
      /* XXX check if the file is actually mentioned in the thunk */
      string path_to_hash = gg::paths::blob_path( arg.substr( GG_HASH_REPLACE.length() ) ).string();
      arg.swap( path_to_hash );
    }
  }

  const roost::path thunk_path = gg::paths::blob_path( hash() );

  // preparing envp
  const vector<string> & f_envars = function_.envars();
  vector<string> envars = {
    "__GG_THUNK_PATH__=" + thunk_path.string(),
    "__GG_DIR__=" + gg::paths::blobs().string(),
    "__GG_ENABLED__=1",
  };

  if ( verbose ) {
    envars.emplace_back( "__GG_VERBOSE__=1" );
  }

  envars.insert( envars.end(), f_envars.begin(), f_envars.end() );

  int retval;

  if ( verbose ) {
    string exec_string = "+ exec(" + hash() + ") {"
                       + roost::rbasename( function_.args().front() ).string()
                       + "}\n";

    cerr << exec_string;
  }

  if ( ( retval = ezexec( gg::paths::blob_path( function_.hash() ).string(),
                          args, envars ) ) < 0 ) {
    throw runtime_error( "execvpe failed" );
  }

  return retval;
}

string Thunk::execution_payload( const Thunk & thunk )
{
  return Thunk::execution_payload( vector<Thunk>{ thunk } );
}

string Thunk::execution_payload( const vector<Thunk> & thunks )
{
  protobuf::ExecutionRequest execution_request;

  for ( const Thunk & thunk : thunks ) {
    string base64_thunk;
    StringSource s( ThunkWriter::serialize_thunk( thunk ), true,
                  new Base64Encoder( new StringSink( base64_thunk ), false ) );

    protobuf::RequestItem request_item;
    request_item.set_thunk_data( base64_thunk );
    request_item.set_thunk_hash( thunk.hash() );
    *execution_request.add_thunks() = request_item;
  }

  execution_request.set_storage_backend( gg::remote::storage_backend_uri() );

  JsonPrintOptions print_options;
  print_options.add_whitespace = false;
  print_options.always_print_primitive_fields = true;

  string ret;
  if ( not MessageToJsonString( execution_request, &ret ).ok() ) {
    throw runtime_error( "cannot create the json output" );
  }

  return ret;
}

protobuf::Thunk Thunk::to_protobuf() const
{
  protobuf::Thunk thunk_proto;

  *thunk_proto.mutable_function() = function_.to_protobuf();

  for ( const string & h : data_.thunks ) { thunk_proto.add_data( h ); }
  for ( const string & h : data_.values ) { thunk_proto.add_data( h ); }
  for ( const string & h : data_.executables ) { thunk_proto.add_data( h ); }

  for ( const string & output : outputs_ ) {
    thunk_proto.add_outputs( output );
  }

  return thunk_proto;
}

void put_file( const roost::path & src, const roost::path & dst )
{
  if ( roost::exists( dst ) ) {
    /* XXX we might want to implement strict checks, like hash check */
    return;
  }

  roost::copy_then_rename( src, dst );
}

bool Thunk::operator==( const Thunk & other ) const
{
  return ( function_ == other.function_ ) and
         ( data_ == other.data_ ) and
         ( outputs_ == other.outputs_ );
}

string Thunk::hash() const
{
  if ( not hash_.initialized() ) {
    hash_.reset( digest::sha256( ThunkWriter::serialize_thunk( *this ) ) );
  }

  return *hash_;
}

string Thunk::executable_hash() const
{
  const string combined_hashes = accumulate( data_.executables.begin(),
                                             data_.executables.end(),
                                             string {} );

  return digest::sha256( combined_hashes, true );
}

void Thunk::update_data( const string & old_hash, const string & new_hash )
{
  hash_.clear();
  data_.thunks.erase( old_hash );

  if ( hash::type( new_hash ) == ObjectType::Thunk ) {
    data_.thunks.insert( new_hash );
  }
  else {
    data_.values.insert( new_hash );
  }

  /* XXX Update the args. */
}

unordered_map<string, Permissions>
Thunk::get_allowed_files() const
{
  unordered_map<string, Permissions> allowed_files;

  for ( const std::string & hash : data_.values ) {
    allowed_files[ gg::paths::blob_path( hash ).string() ] = { true, false, false };
  }

  for ( const std::string & hash : data_.executables ) {
    allowed_files[ gg::paths::blob_path( hash ).string() ] = { true, false, true };
  }

  allowed_files[ gg::paths::blobs().string() ] = { true, false, false };

  for ( const string & output : outputs_ ) {
    allowed_files[ output ] = { true, true, false };
  }

  return allowed_files;
}

size_t Thunk::infiles_size( const bool include_executables ) const
{
  size_t total_size = 0;

  for ( const string & hash : data_.values ) {
    total_size += gg::hash::size( hash );
  }

  if ( include_executables ) {
    for ( const string & hash : data_.executables ) {
      total_size += gg::hash::size( hash );
    }
  }

  return total_size;
}
