/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include <string>
#include <vector>
#include <functional>

#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS

#include "gg.pb.h"

namespace gg {
  namespace thunk {

    static const std::string MAGIC_NUMBER = "##GGTHUNK##";

    class InFile
    {
    private:

      std::string filename_;
      std::string hash_;
      size_t order_;

      size_t compute_order() const;

    public:
      InFile( const std::string & filename );
      InFile( const std::string & filename, const std::string & hash, const size_t order );
      InFile( const gg::protobuf::InFile & infile_proto );

      std::string filename() const { return filename_; }
      std::string hash() const { return hash_; }
      size_t order() const { return order_; }

      std::string to_envar( const std::string & root_dir ) const;

      gg::protobuf::InFile to_protobuf() const;

      bool operator==( const InFile & other ) const;
      bool operator!=( const InFile & other ) const { return not operator==( other ); }

      static std::string compute_hash( const std::string & filename );
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
      Function( const std::vector<std::string> & cmd, const std::string & hash );
      Function( const gg::protobuf::Function & func_proto );

      std::string exe() const { return exe_; }
      std::vector<std::string> args() const { return args_; }
      std::string hash() const { return exe_hash_; }

      gg::protobuf::Function to_protobuf() const;

      bool operator==( const Function & other ) const;
      bool operator!=( const Function & other ) const { return not operator==( other ); }
    };


    class Thunk
    {
    private:
      std::string outfile_;
      Function function_;
      std::vector<InFile> infiles_;
      size_t order_;

      size_t compute_order() const;

    public:
      Thunk( const std::string & outfile, const Function & function,
             const std::vector<InFile> & infiles );

      Thunk( const gg::protobuf::Thunk & thunk_proto );

      int execute( const std::string & root_dir = ".gg/" ) const;

      std::string outfile() const { return outfile_; }
      Function function() const { return function_; }
      std::vector<InFile> infiles() const { return infiles_; }
      size_t order() const { return order_; }

      gg::protobuf::Thunk to_protobuf() const;

      void collect_infiles( const boost::filesystem::path & gg_dir ) const;

      /* this function will collect all of the infiles in .gg directory, and
         will store two copies for the thunk, both in the working directory
         and .gg directory. */
      void store( const boost::filesystem::path & gg_dir ) const;

      bool operator==( const Thunk & other ) const;
      bool operator!=( const Thunk & other ) const { return not operator==( other ); }
    };
  }
}
