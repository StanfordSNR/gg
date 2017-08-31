/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef STRICT_CONVERSIONS_HH
#define STRICT_CONVERSIONS_HH

#include <string>

long int strict_atoi( const std::string & str, const int base = 10 );
double strict_atof( const std::string & str );

#endif /* STRICT_CONVERSIONS_HH */
