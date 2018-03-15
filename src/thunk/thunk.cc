/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "thunk.hh"

#include <unistd.h>
#include <cstring>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <regex>
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

string thunk::data_placeholder( const string & hash )
{
  return DATA_PLACEHOLDER_START + hash + DATA_PLACEHOLDER_END;
}

string data_to_string( const pair<const string, string> & item )
{
  if ( item.second.length() > 0 ) {
    return item.first + '=' + item.second;
  }
  else {
    return item.first;
  }
}

pair<const string, string> string_to_data( const string & str )
{
  auto eqpos = str.find( '=' );
  if ( eqpos == string::npos ) {
    return make_pair( str, string {} );
  }
  else {
    return make_pair( str.substr( 0, eqpos ), str.substr( eqpos + 1 ) );
  }
}

void Thunk::throw_if_error() const
{
  if ( outputs_.size() == 0 ) {
    throw runtime_error( "a thunk needs at least one output" );
  }

  if ( executables_.size() == 0 ) {
    throw runtime_error( "a thunk needs at least one executable" );
  }

  if ( function_.args().size() == 0 ) {
    throw runtime_error( "a thunk needs at least one argument (argv0)" );
  }
}

Thunk::Thunk( const Function & function,
              const vector<DataItem> & data,
              const vector<DataItem> & executables,
              const vector<string> & outputs )
  : function_( function ),
    values_(),
    thunks_(),
    executables_( executables.cbegin(), executables.cend() ), outputs_( outputs )
{
  for ( const DataItem & item : data ) {
    switch ( hash::type( item.first ) ) {
    case ObjectType::Value: values_.emplace( item ); break;
    case ObjectType::Thunk: thunks_.emplace( item ); break;
    }
  }

  throw_if_error();
}

Thunk::Thunk( Function && function,
              vector<DataItem> && data,
              vector<DataItem> && executables,
              vector<string> && outputs )
  : function_( move( function ) ), values_(), thunks_(), executables_(),
    outputs_( move( outputs ) )
{
  for ( DataItem & item : data ) {
    switch ( hash::type( item.first ) ) {
    case ObjectType::Value: values_.emplace( move( item ) ); break;
    case ObjectType::Thunk: thunks_.emplace( move( item ) ); break;
    }
  }

  for ( DataItem & item : executables ) {
    executables_.emplace( move( item ) ) ;
  }

  throw_if_error();
}

Thunk::Thunk( const gg::protobuf::Thunk & thunk_proto )
  : function_( thunk_proto.function() ),
    values_(),
    thunks_(),
    executables_(),
    outputs_( thunk_proto.outputs().cbegin(), thunk_proto.outputs().cend() )
{
  for ( const string & item : thunk_proto.data() ) {
    switch ( hash::type( item ) ) {
    case ObjectType::Value: values_.emplace( string_to_data( item ) ); break;
    case ObjectType::Thunk: thunks_.emplace( string_to_data( item ) ); break;
    }
  }

  for ( const string & item : thunk_proto.executables() ) {
    executables_.emplace( string_to_data( item ) );
  }

  throw_if_error();
}

int Thunk::execute() const
{
  if ( thunks_.size() != 0 ) {
    throw runtime_error( "cannot execute thunk with unresolved dependencies" );
  }

  bool verbose = ( getenv( "GG_VERBOSE" ) != nullptr );

  // preparing argv
  vector<string> args = function_.args();
  vector<string> envars { function_.envars() };

  auto replace_data_placeholder =
    []( string & str ) -> void
    {
      if ( regex_search( str, DATA_PLACEHOLDER_REGEX ) ) {
        string new_path = regex_replace( str, DATA_PLACEHOLDER_REGEX, gg::paths::blob_path( "$1" ).string() );
        swap( str, new_path );
      }
    };

  /* do we need to replace a hash placeholder with the actual path? */
  for ( string & arg : args ) {
    replace_data_placeholder( arg );
  }

  for ( string & envar : envars ) {
    replace_data_placeholder( envar );
  }

  const roost::path thunk_path = gg::paths::blob_path( hash() );

  // preparing envp
  envars.insert( envars.end(), {
    "__GG_THUNK_PATH__=" + thunk_path.string(),
    "__GG_DIR__=" + gg::paths::blobs().string(),
    "__GG_ENABLED__=1",
  } );

  if ( verbose ) {
    envars.emplace_back( "__GG_VERBOSE__=1" );
  }

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

  for ( const auto & h : thunks_ ) { thunk_proto.add_data( data_to_string( h ) ); }
  for ( const auto & h : values_ ) { thunk_proto.add_data( data_to_string( h ) ); }
  for ( const auto & h : executables_ ) { thunk_proto.add_executables( data_to_string( h ) ); }
  for ( const string & output : outputs_ ) { thunk_proto.add_outputs( output ); }

  return thunk_proto;
}

bool Thunk::operator==( const Thunk & other ) const
{
  return ( function_ == other.function_ ) and
         ( values_ == other.values_ ) and
         ( thunks_ == other.thunks_ ) and
         ( executables_ == other.executables_ ) and
         ( outputs_ == other.outputs_ );
}

string Thunk::hash() const
{
  if ( not hash_.initialized() ) {
    hash_.reset( gg::hash::compute( ThunkWriter::serialize_thunk( *this ),
                                    ObjectType::Thunk ) );
  }

  return *hash_;
}

string Thunk::executable_hash() const
{
  const string combined_hashes = accumulate( executables_.begin(),
                                             executables_.end(),
                                             string {},
                                             []( const auto & a, const auto & b )
                                             {
                                               return a + b.first;
                                             } );

  return digest::sha256( combined_hashes );
}

void Thunk::update_data( const string & old_hash, const string & new_hash )
{
  hash_.clear();
  string old_name { move( thunks_.at( old_hash ) ) };
  thunks_.erase( old_hash );

  switch ( hash::type( new_hash ) ) {
  case ObjectType::Thunk: thunks_.insert( { new_hash, old_name } ); break;
  case ObjectType::Value: values_.insert( { new_hash, old_name } ); break;
  }

  /* XXX Update the args. */
}

unordered_map<string, Permissions>
Thunk::get_allowed_files() const
{
  unordered_map<string, Permissions> allowed_files;

  for ( const DataItem & item : values_ ) {
    allowed_files[ gg::paths::blob_path( item.first ).string() ] = { true, false, false };
  }

  for ( const DataItem & item : executables_ ) {
    allowed_files[ gg::paths::blob_path( item.first ).string() ] = { true, false, true };
  }

  allowed_files[ gg::paths::blobs().string() ] = { true, false, false };
  allowed_files[ gg::paths::blob_path( hash() ).string() ] = { true, false, false };

  for ( const string & output : outputs_ ) {
    allowed_files[ output ] = { true, true, false };
  }

  return allowed_files;
}

size_t Thunk::infiles_size( const bool include_executables ) const
{
  size_t total_size = 0;

  for ( const DataItem & item : values_ ) {
    total_size += gg::hash::size( item.first );
  }

  if ( include_executables ) {
    for ( const DataItem & item : executables_ ) {
      total_size += gg::hash::size( item.first );
    }
  }

  return total_size;
}
