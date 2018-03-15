/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "cli_description.hh"

#include <stdexcept>

#include "util/tokenize.hh"

using namespace std;

CLIDescription::CLIDescription( const string & description )
{
  vector<string> tokens = split( description, " " );

  if ( tokens.size() == 0 ) {
    throw runtime_error( "invalid description" );
  }

  size_t arg_ind = 0;
  size_t opt_val = 1000;

  for ( size_t i = 0; i < tokens.size(); i++ ) {
    const string & token = tokens[ i ];

    if ( i == 0 ) {
      target_bin_ = token;
    }
    else if ( token[ 0 ] == '-' ) {
      vector<string> segments = split( token, "=" );

      if ( segments.size() != 2 ) {
        throw runtime_error( "invalid option specification" );
      }

      CLIOption option;
      option.value = opt_val++;

      if ( segments[ 1 ] == "@outfile" ) {
        option.type = CLIOption::Type::OutFile;
      }
      else if ( segments[ 1 ] == "@infile" ) {
        option.type = CLIOption::Type::InFile;
      }
      else {
        option.type = CLIOption::Type::DoNotCare;
      }

      for ( auto & optname : split( segments[ 0 ], "," ) ) {
        if ( optname.compare( 0, 2, "--" ) == 0 ) {
          option.long_opt.reset( optname.substr( 2 ) );
        }
        else if ( optname[ 0 ] == '-' and optname.length() > 1 ) {
          option.value = static_cast<int>( optname[ 1 ] );
          option.short_opt.reset( optname[ 1 ] );
        }
        else {
          throw runtime_error( "invalid option specification" );
        }
      }

      options_.push_back( option );
    }
    else if ( token == "@infile" ) {
      infile_args_.push_back( arg_ind++ );
    }
    else if ( token == "@outfile" ) {
      outfile_args_.push_back( arg_ind++ );
    }
    else {
      throw runtime_error( "unexpected token in description" );
    }
  }
}
