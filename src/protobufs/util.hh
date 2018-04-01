/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef PROTOBUF_UTIL_HH
#define PROTOBUF_UTIL_HH

#include <string>
#include <stdexcept>
#include <google/protobuf/util/json_util.h>

namespace protoutil {

  template<class ProtobufType>
  std::string to_json( const ProtobufType & protobuf, const bool pretty_print = false )
  {
    using namespace google::protobuf::util;
    JsonPrintOptions print_options;
    print_options.add_whitespace = pretty_print;
    print_options.always_print_primitive_fields = true;

    std::string ret;
    if ( not MessageToJsonString( protobuf, &ret ).ok() ) {
      throw std::runtime_error( "cannot convert protobuf to json" );
    }

    return ret;
  }

}

#endif /* PROTOBUF_UTIL_HH */
