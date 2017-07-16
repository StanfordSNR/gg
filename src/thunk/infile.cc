/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <cctype>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <boost/filesystem.hpp>

#include "optional.hh"
#include "thunk.hh"
#include "thunk_reader.hh"
#include "digest.hh"

using namespace std;
using namespace gg;
using namespace gg::thunk;

namespace fs = boost::filesystem;

InFile::InFile( const string & filename )
  : InFile( filename, filename )
{}

InFile::InFile( const string & filename, const string & real_filename )
  : filename_( filename ), real_filename_( real_filename ),
    hash_( compute_hash( real_filename_ ) ), order_( compute_order( real_filename_ ) ),
    size_( compute_size( real_filename_ ) )
{}

InFile::InFile( const string & filename, const string & real_filename,
                const string & hash, const size_t order, const off_t size )
  : filename_( filename ), real_filename_( real_filename ), hash_( hash ),
    order_( order ), size_( size )
{}

InFile::InFile( const string & filename, const string & real_filename,
                const string & hash, const size_t order )
  : InFile( filename, real_filename, hash, order, compute_size( real_filename ) )
{}

InFile::InFile( const string & filename, const string & real_filename,
                const string & hash )
  : InFile( filename, real_filename, hash, compute_order( real_filename ),
            compute_size( real_filename ) )
{}

InFile::InFile( const protobuf::InFile & infile_proto )
  : filename_( infile_proto.filename() ), real_filename_( filename_ ),
    hash_( infile_proto.hash() ), order_( infile_proto.order() ),
    size_( 0 ) /* XXX NEEDS TO BE TAKEN CARE OF */
{}

size_t InFile::compute_order( const string & filename )
{
  ThunkReader thunk_reader { filename };

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

off_t InFile::compute_size( const string & filename )
{
  return fs::file_size( filename );
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
