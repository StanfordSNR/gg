#include "mime_type.hh"
#include "tokenize.hh"
#include "exception.hh"

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
