/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "temp_dir.hh"

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <cassert>

#include "temp_file.hh"
#include "exception.hh"

using namespace std;

UniqueDirectory::UniqueDirectory( const string & dirname_template )
  : mutable_temp_dirname_( to_mutable( dirname_template + ".XXXXXX" ) ),
    owns_dir_( true )
{
  if ( mkdtemp( &mutable_temp_dirname_[ 0 ] ) == NULL ) {
    throw unix_error( "mkdtemp " + name() );
  }
}

/* unlike UniqueDirectory, a TempDirectory is deleted when object destroyed */
TempDirectory::~TempDirectory()
{
  try {
    remove();
  } catch (unix_error & e) {
    cerr << "Warning: " << e.what() << endl;
  }
}

void TempDirectory::remove()
{
  if ( not owns_dir_ ) { return; }
  owns_dir_ = false;
  CheckSystemCall( "rmdir " + name(), rmdir( name().c_str() ) );
}

UniqueDirectory::UniqueDirectory( UniqueDirectory && other )
  : mutable_temp_dirname_( other.mutable_temp_dirname_ ),
    owns_dir_( true )
{
  other.owns_dir_ = false;
}

string UniqueDirectory::name( void ) const
{
  assert( mutable_temp_dirname_.size() > 1 );
  return string( mutable_temp_dirname_.begin(), mutable_temp_dirname_.end() - 1 );
}
