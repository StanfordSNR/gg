/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#pragma once

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <sys/types.h>

#include "sandbox.hh"
#include "path.hh"
#include "gg.pb.h"

namespace gg {
  namespace thunk {

    static const std::string MAGIC_NUMBER = "##GGTHUNK##";

    static const std::string BEGIN_REPLACE = "__GG_BEGIN_REPLACE__";
    static const std::string END_REPLACE = "__GG_END_REPLACE__";

    class InFile
    {
    public:
      enum class Type
      {
        FILE = 0,
        EXECUTABLE = 1,
        DUMMY_DIRECTORY = 2,
      };

    private:
      /* name of the file in the thunk */
      std::string filename_;
      /* where the actual content is */
      std::string real_filename_;
      std::string content_hash_;
      size_t order_;
      off_t size_;
      Type type_ { Type::FILE };

      /* Checks if the real_filename_ is actually a placeholder */
      void fill_file_info();

    public:
      InFile( const std::string & filename, const std::string & real_filename = {},
              const Type type = Type::FILE );

      InFile( const std::string & filename, const std::string & real_filename,
              const std::string & hash, const size_t order, const off_t size,
              const Type type = Type::FILE );

      InFile( const gg::protobuf::InFile & infile_proto );

      const std::string & filename() const { return filename_; }
      const std::string & real_filename() const { return real_filename_; }
      const std::string & content_hash() const { return content_hash_; }
      size_t order() const { return order_; }
      off_t size() const { return size_; }
      Type type() const { return type_; }

      gg::protobuf::InFile to_protobuf() const;

      bool operator==( const InFile & other ) const;
      bool operator!=( const InFile & other ) const { return not operator==( other ); }

      static size_t compute_order( const std::string & filename );
      static std::string compute_hash( const std::string & filename );
      static off_t compute_size( const std::string & filename );

      /* does the same file exist, with same contents, in the filesystem? */
      bool matches_filesystem() const;
    };

    class Function
    {
    private:
      std::string exe_ {};
      std::vector<std::string> args_;
      std::string exe_hash_ {};
      std::vector<std::string> envars_ {};

      void parse_cmd();

      static std::string hash_exe( const std::string & exe );

    public:
      Function( const std::string & exe, const std::vector<std::string> & cmd,
                const std::vector<std::string> & envars );
      Function( const std::string & exe, const std::vector<std::string> & cmd,
                const std::vector<std::string> & envars, const std::string & hash );
      Function( const gg::protobuf::Function & func_proto );

      const std::string & exe() const { return exe_; }
      const std::vector<std::string> & args() const { return args_; }
      const std::string & hash() const { return exe_hash_; }
      const std::vector<std::string> & envars() const { return envars_; }

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
      std::string filename_to_hash( const std::string & filename ) const;

    public:
      Thunk( const std::string & outfile, const Function & function,
             const std::vector<InFile> & infiles );

      Thunk( const gg::protobuf::Thunk & thunk_proto );

      int execute( const std::string & thunk_hash ) const;

      const std::string & outfile() const { return outfile_; }
      const Function & function() const { return function_; }
      const std::vector<InFile> & infiles() const { return infiles_; }
      size_t order() const { return order_; }

      gg::protobuf::Thunk to_protobuf() const;

      void collect_infiles() const;

      /* this function will collect all of the infiles in .gg directory, and
         will store two copies for the thunk, both in the working directory
         and .gg directory. It returns the hash. */
      std::string store( const bool create_placeholder = true ) const;

      bool operator==( const Thunk & other ) const;
      bool operator!=( const Thunk & other ) const { return not operator==( other ); }

      std::string executable_hash() const;

      void update_infile( const std::string & old_hash,
                          const std::string & new_hash,
                          const size_t new_order,
                          const off_t new_size );

      off_t infiles_size( const bool include_executables = true ) const;

      /* Returns a list of files that can be accessed while executing this
         thunk. */
      std::unordered_map<std::string, Permissions>
      get_allowed_files( const std::string & thunk_hash ) const;
    };
  }
}
