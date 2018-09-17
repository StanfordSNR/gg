/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "thunk.hh"

#include <unistd.h>
#include <cstring>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <regex>

#include "protobufs/util.hh"
#include "thunk/ggutils.hh"
#include "thunk/factory.hh"
#include "thunk/placeholder.hh"
#include "thunk/thunk_writer.hh"
#include "util/digest.hh"
#include "util/system_runner.hh"
#include "util/temp_file.hh"

using namespace std;
using namespace gg;
using namespace gg::thunk;
using namespace CryptoPP;

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

pair<const string, string> Thunk::string_to_data( const string & str )
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
    executables_( executables.cbegin(), executables.cend() ),
    outputs_( outputs )
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

Thunk::Thunk( Function && function,
              vector<DataItem> && values,
              vector<DataItem> && thunks,
              vector<DataItem> && executables,
              vector<string> && outputs )
  : function_( move( function ) ), values_(), thunks_(), executables_(),
    outputs_( move( outputs ) )
{
  for ( DataItem & item : values ) {
    values_.emplace( move( item ) );
  }

  for ( DataItem & item : thunks ) {
    thunks_.emplace( move( item ) );
  }

  for ( DataItem & item : executables ) {
    executables_.emplace( move( item ) ) ;
  }

  throw_if_error();
}

Thunk::Thunk( Function && function, DataList && values,
              DataList && thunks, DataList && executables,
              vector<string> && outputs )
  : function_( move( function ) ), values_( move( values ) ),
    thunks_( move( thunks ) ), executables_( move( executables ) ),
    outputs_( move( outputs ) )
{
  throw_if_error();
}

Thunk::Thunk( const gg::protobuf::Thunk & thunk_proto )
  : function_( thunk_proto.function() ),
    values_(),
    thunks_(),
    executables_(),
    outputs_( thunk_proto.outputs().cbegin(), thunk_proto.outputs().cend() ),
    timeout_( thunk_proto.timeout() )
{
  for ( const string & item : thunk_proto.values() ) {
    values_.emplace( string_to_data( item ) );
  }

  for ( const string & item : thunk_proto.thunks() ) {
    thunks_.emplace( string_to_data( item ) );
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
        string new_path = regex_replace( str, DATA_PLACEHOLDER_REGEX, gg::paths::blob( "$1" ).string() );
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

  const roost::path thunk_path = gg::paths::blob( hash() );

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

  if ( ( retval = ezexec( gg::paths::blob( function_.hash() ).string(),
                          args, envars ) ) < 0 ) {
    throw runtime_error( "execvpe failed" );
  }

  return retval;
}

protobuf::RequestItem Thunk::execution_request( const Thunk & thunk )
{
  protobuf::RequestItem request_item;

  string base64_thunk;
  StringSource s( ThunkWriter::serialize( thunk ), true,
                  new Base64Encoder( new StringSink( base64_thunk ), false ) );

  request_item.set_data( base64_thunk );
  request_item.set_hash( thunk.hash() );

  for ( const string & output : thunk.outputs() ) {
    request_item.add_outputs( output );
  }

  return request_item;
}

string Thunk::execution_payload( const Thunk & thunk )
{
  return Thunk::execution_payload( vector<Thunk>{ thunk } );
}

string Thunk::execution_payload( const vector<Thunk> & thunks )
{
  static const bool timelog = ( getenv( "GG_TIMELOG" ) != nullptr );
  protobuf::ExecutionRequest request;

  for ( const Thunk & thunk : thunks ) {
    *request.add_thunks() = execution_request( thunk );
  }

  request.set_storage_backend( gg::remote::storage_backend_uri() );
  request.set_timelog( timelog );
  return protoutil::to_json( request );
}

protobuf::Thunk Thunk::to_protobuf() const
{
  protobuf::Thunk thunk_proto;

  *thunk_proto.mutable_function() = function_.to_protobuf();

  for ( const auto & h : values_ ) { thunk_proto.add_values( data_to_string( h ) ); }
  for ( const auto & h : thunks_ ) { thunk_proto.add_thunks( data_to_string( h ) ); }
  for ( const auto & h : executables_ ) { thunk_proto.add_executables( data_to_string( h ) ); }
  for ( const string & output : outputs_ ) { thunk_proto.add_outputs( output ); }

  thunk_proto.set_timeout( timeout_.count() );

  return thunk_proto;
}

bool Thunk::operator==( const Thunk & other ) const
{
  return ( function_ == other.function_ ) and
         ( values_ == other.values_ ) and
         ( thunks_ == other.thunks_ ) and
         ( executables_ == other.executables_ ) and
         ( outputs_ == other.outputs_ ) and
         ( timeout_ == other.timeout_ );
}

void Thunk::set_timeout( const std::chrono::milliseconds & timeout )
{
  hash_.clear();
  timeout_ = timeout;
}

string Thunk::hash() const
{
  if ( not hash_.initialized() ) {
    hash_.reset( gg::hash::compute( ThunkWriter::serialize( *this ),
                                    ObjectType::Thunk ) );
  }

  return *hash_;
}

string Thunk::output_hash( const string & tag ) const
{
  return gg::hash::for_output( hash(), tag );
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

void Thunk::update_data( const string & original_hash,
                         const vector<ThunkOutput> & outputs )
{
  hash_.clear(); /* invalidating the cached hash */

  bool first_output = true;

  /* NOTE Okay, to prevent a performance hit here, we say that the first output
  must never be referenced with its tag */
  for ( const auto & output : outputs ) {
    const string old_hash = ( first_output )
                          ? original_hash
                          : hash::for_output( original_hash, output.tag );

    const string new_hash = ( first_output or hash::type( output.hash ) == ObjectType::Value )
                          ? output.hash
                          : hash::for_output( output.hash, output.tag );

    auto result = thunks_.equal_range( old_hash );

    vector<string> old_names;

    for ( auto it = result.first; it != result.second; ) {
      old_names.emplace_back( move( it->second ) );
      it = thunks_.erase( it );
    }

    for ( const auto & old_name : old_names ) {
      switch ( hash::type( new_hash ) ) {
      case ObjectType::Thunk: thunks_.insert( { new_hash, old_name } ); break;
      case ObjectType::Value: values_.insert( { new_hash, old_name } ); break;
      }
    }

    /* let's update the args/envs as necessary */
    const string srcstr = data_placeholder( old_hash );
    const string dststr = data_placeholder( new_hash );

    auto update_placeholder =
      []( string & str, const string & src, const string & dst )
      {
        size_t index = 0;

        while ( true ) {
          index = str.find( src, index );
          if ( index == string::npos ) break;
          str.replace( index, src.length(), dst );
          index += dst.length();
        }
      };

    for ( string & arg : function_.args() ) {
      update_placeholder( arg, srcstr, dststr );
    }

    for ( string & envar : function_.envars() ) {
      update_placeholder( envar, srcstr, dststr );
    }

    first_output = false;
  }
}

unordered_map<string, Permissions>
Thunk::get_allowed_files() const
{
  unordered_map<string, Permissions> allowed_files;

  for ( const DataItem & item : values_ ) {
    allowed_files[ gg::paths::blob( item.first ).string() ] = { true, false, false };
  }

  for ( const DataItem & item : executables_ ) {
    allowed_files[ gg::paths::blob( item.first ).string() ] = { true, false, true };
  }

  allowed_files[ gg::paths::blobs().string() ] = { true, false, false };
  allowed_files[ gg::paths::blob( hash() ).string() ] = { true, false, false };

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

bool Thunk::matches_filesystem( const DataItem & item )
{
  const string & hash = item.first;
  const string & filename = item.second;

  if ( filename.length() == 0 ) {
    return false;
  }

  if ( not roost::exists( filename ) ) {
    return false;
  }

  if ( gg::hash::size( hash ) != roost::file_size( filename ) ) {
    return false;
  }

  return hash == gg::hash::file( filename, gg::hash::type( hash ) );
}
