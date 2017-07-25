/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "path.hh"

using namespace std;
using namespace roost;

Path::Path( const std::string & path )
  : path_( path )
{}

Path::Path( const boost::filesystem::path & boost_path_ )
  : path_( boost_path_ )
{}

Path Path::lexically_normal() const
{
  return path_.lexically_normal();
}

const string & Path::string() const
{
  return path_.string();
}

bool exists( const Path & path )
{
  return boost::filesystem::exists( path.boost_path() );
}

size_t file_size( const Path & path )
{
  return boost::filesystem::file_size( path.boost_path() );
}

Path absolute( const Path & path )
{
  return boost::filesystem::absolute( path.boost_path() );
}

void copy_file( const Path & src, const Path & dst )
{
  return boost::filesystem::copy_file( src.boost_path(),
				       dst.boost_path(),
				       boost::filesystem::copy_option::overwrite_if_exists );
}
