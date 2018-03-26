/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <stdexcept>

#include "util/path.hh"

using namespace std;

void test_lexically_normal( const string & pathname, const string & expected_path )
{
  string normalized_path = roost::path( pathname ).lexically_normal().string();
  if ( expected_path != normalized_path ) {
    throw runtime_error( "lexically_normal() test failed: " + pathname );
  }
}

int main()
{
  test_lexically_normal( "/", "/" );

  test_lexically_normal( "./", "" );

  test_lexically_normal( ".", "" );

  test_lexically_normal( "/home/sadjad", "/home/sadjad" );

  test_lexically_normal( "./home/sadjad", "home/sadjad" );

  test_lexically_normal( "///home////////sadja....d///////", "/home/sadja....d" );

  test_lexically_normal( "/../home/sadjad", "/home/sadjad" );

  test_lexically_normal( "/home/../sadjad//john", "/sadjad/john" );

  test_lexically_normal(
    "/home/sadjad/../../usr/bin/../lib/gcc/../ncurses/./././../g++/./stdlib.so",
    "/usr/lib/g++/stdlib.so" );

  test_lexically_normal(
    "/home/sadjad/../../../././../../././../../../../home/.././etc/john/sadjad",
    "/etc/john/sadjad" );

  test_lexically_normal(
    "../../../home/../etc/../dev/../../proc/cpuinfo",
    "../../../../proc/cpuinfo" );

  test_lexically_normal( "../../../../../..", "../../../../../.." );

  test_lexically_normal(
    "/home/sadjad/projects/gg/././../../../../../../../usr/bin/./gcc",
    "/usr/bin/gcc" );

  test_lexically_normal( "./home/sadjad/.", "home/sadjad" );

  test_lexically_normal( "./home/sadjad//..", "home" );

  test_lexically_normal(
    "../../../home/../../../../../../../../../dev/video0",
    "../../../../../../../../../../../dev/video0" );

  return 0;
}
