/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "manifest.hh"

#include <sstream>

using namespace std;

void FileManifest::add_filename_to_hash( const string & filename, const string & hash )
{
  filename_to_hashes_.emplace_back( filename, hash );
}

void FileManifest::add_dummy_directory( const string & dummy_dir )
{
  dummy_directories_.emplace_back( dummy_dir );
}

void FileManifest::add_output_tag( const string & filename, const string & tag )
{
  output_tags_.emplace_back( filename, tag );
}

string FileManifest::serialize() const
{
  ostringstream sout;

  for ( const auto & entry : filename_to_hashes_ ) {
    sout << "F " << entry.first << ' ' << entry.second << endl;
  }

  for ( const auto & entry : dummy_directories_ ) {
    sout << "D " << entry << endl;
  }

  for ( const auto & entry : output_tags_ ) {
    sout << "O " << entry.first << ' ' << entry.second << endl;
  }

  return sout.str();
}
