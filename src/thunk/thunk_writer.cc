/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "thunk_writer.hh"

#include <iostream>
#include <cajun/json/writer.h>

using namespace std;

bool ThunkWriter::write_thunk( Thunk thunk )
{
  std::stringstream stream;
  json::Writer::Write( thunk.to_json(), stream );
  cout << stream.str() << endl;
  return true;
}
