/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <cctype>
#include <fstream>
#include <sstream>
#include <iomanip>

#include "optional.hh"
#include "thunk.hh"
#include "thunk_reader.hh"
#include "digest.hh"

using namespace std;
using namespace gg;
using namespace gg::thunk;

InFile::InFile( const string & filename )
  : filename_( filename ), hash_( compute_hash( filename ) ), order_( compute_order() )
{}

InFile::InFile( const string & filename, const string & hash, const size_t order )
  : filename_( filename ), hash_( hash ), order_( order )
{}

InFile::InFile( const protobuf::InFile & infile_proto )
  : filename_( infile_proto.filename() ), hash_( infile_proto.hash() ),
    order_( infile_proto.order() )
{}

size_t InFile::compute_order() const
{
  ThunkReader thunk_reader { filename_ };

  // check if the file has the gg-thunk magic number
  if ( not thunk_reader.is_thunk() ) {
    /* not a thunk file, so the order is 0 */
    return 0;
  }
  else {
    return thunk_reader.read_thunk().order();
  }
}

string InFile::compute_hash( const string & filename )
{
  // TODO : Check if file exists!
  ifstream file( filename, ifstream::binary );

  if( file.fail() ){
    throw runtime_error( "File " + filename + " does not exist." );
  }

  return digest::SHA256( file ).hexdigest();
}

string InFile::to_envar( const string & root_dir ) const
{
  string result( filename_ );

  for ( size_t i = 0; i < result.length(); i++ ) {
    if ( not ( isalnum( result[ i ] ) or result[ i ] == '_' ) ) {
      result[ i ] = '_';
    }
  }

  result += '=';
  result += root_dir + hash_;

  return result;
}

protobuf::InFile InFile::to_protobuf() const
{
  protobuf::InFile infile;

  infile.set_filename( filename_ );
  infile.set_hash( hash_ );
  infile.set_order( order_ );

  return infile;
}

bool InFile::operator==( const InFile & other ) const
{
  return ( filename_ == other.filename_ ) and
         ( hash_ == other.hash_ ) and
         ( order_ == other.order_ );
}
