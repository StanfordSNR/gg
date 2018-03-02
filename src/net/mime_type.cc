/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "mime_type.hh"
#include "util/exception.hh"
#include "util/tokenize.hh"

using namespace std;

MIMEType::MIMEType( const string & content_type )
  : type_(), parameters_()
{
  auto type_and_parameters = split( content_type, ";" );
  if ( type_and_parameters.size() == 0
       or type_and_parameters.at( 0 ).empty() ) {
    throw runtime_error( "MIMEType: invalid MIME media-type string" );
  }

  type_ = type_and_parameters.at( 0 );
  /* XXX don't parse the parameters for now */
}
