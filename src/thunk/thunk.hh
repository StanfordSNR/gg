/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include <string>
#include <vector>

#include "gg.pb.h"

namespace gg {
  namespace thunk {

    class InFile
    {
    private:

      std::string filename_;
      std::string hash_;
      int order_;

      void is_thunk();
      void get_thunk_order();

      static std::string compute_hash( const std::string & filename );

    public:
      // TODO : This currently does not check the order (if it is a thunk or not)
      InFile( const std::string & filename );
      InFile( const gg::protobuf::InFile & infile_proto );

      gg::protobuf::InFile to_protobuf() const;
    };

    class Function
    {
    private:
      std::string exe_ {};
      std::vector<std::string> args_; // args_ contains the exe_ in the first argument without the full path
      std::string exe_hash_ {};

      void parse_cmd();

      static std::string get_exe_path( const std::string & exe );
      static std::string hash_exe( const std::string & exe );

    public:
      Function( const std::vector<std::string> & cmd );
      Function( const gg::protobuf::Function & func_proto );

      gg::protobuf::Function to_protobuf() const;
    };


    class Thunk
    {
    private:
      std::string outfile_;
      Function function_;
      std::vector<InFile> infiles_;
      int order_; // TODO : check infiles to figure out order

    public:
      Thunk( const std::string & outfile, const Function & function,
             const std::vector<InFile> & infiles );

      Thunk( const gg::protobuf::Thunk & thunk_proto );

      std::string outfile() const { return outfile_; }

      gg::protobuf::Thunk to_protobuf() const;
    };
  }
}
