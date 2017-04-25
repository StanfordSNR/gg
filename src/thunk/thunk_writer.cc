/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "thunk_writer.hh"

#include <iostream>
#include <cajun/json/writer.h>
#include <fstream>
// TODO : should local header files be first or system header files?

using namespace std;

bool ThunkWriter::write_thunk( Thunk thunk )
{
  // TODO : Remove print statements
  std::stringstream stream;
  json::Writer::Write( thunk.to_json(), stream );
  cout << stream.str() << endl;

  ofstream outfile;
  outfile.open( thunk.get_outfile() );
  outfile << stream.rdbuf();
  outfile.close();

  return true;
}
