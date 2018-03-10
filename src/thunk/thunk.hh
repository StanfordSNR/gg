/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef THUNK_HH
#define THUNK_HH

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <limits>
#include <sys/types.h>
#include <crypto++/base64.h>
#include <crypto++/files.h>

#include "protobufs/gg.pb.h"
#include "sandbox/sandbox.hh"
#include "util/optional.hh"
#include "util/path.hh"

namespace gg {

  enum class ObjectType : char
  {
    Value = 'V',
    Executable = 'X',
    Thunk = 'T',
  };

  namespace thunk {

    static const std::string MAGIC_NUMBER = "##GGTHUNK##";
    static const std::string BEGIN_REPLACE = "__GG_BEGIN_REPLACE__";
    static const std::string END_REPLACE = "__GG_END_REPLACE__";
    static const std::string GG_HASH_REPLACE = "@@GG_HASH@@";

    class Function
    {
    private:
      std::string hash_ {};
      std::vector<std::string> args_;
      std::vector<std::string> envars_ {};

    public:
      Function( const std::string & hash, const std::vector<std::string> & args,
                const std::vector<std::string> & envars );

      Function( const gg::protobuf::Function & func_proto );

      const std::string & hash() const { return hash_; }
      const std::vector<std::string> & args() const { return args_; }
      const std::vector<std::string> & envars() const { return envars_; }

      gg::protobuf::Function to_protobuf() const;

      bool operator==( const Function & other ) const;
      bool operator!=( const Function & other ) const { return not operator==( other ); }
    };

    class Thunk
    {
    private:
      struct Data {
        Data( const std::vector<std::string> & data );

        template<class Iterator>
        Data( Iterator begin, Iterator end );

        std::set<std::string> values {};      /* prefixed V */
        std::set<std::string> thunks {};      /* prefixed T */
        std::set<std::string> executables {}; /* prefixed X */

        bool operator==( const Data & other ) const;
        bool operator!=( const Data & other ) const { return not operator==( other ); }
      };

      Function function_;
      Data data_;
      std::vector<std::string> outputs_;

      mutable Optional<std::string> hash_ {};

      std::string filename_to_hash( const std::string & filename ) const;

    public:
      Thunk( const Function & function,
             const std::vector<std::string> & data,
             const std::vector<std::string> & outputs );

      Thunk( const gg::protobuf::Thunk & thunk_proto );

      int execute() const;

      static std::string execution_payload( const Thunk & thunk );
      static std::string execution_payload( const std::vector<Thunk> & thunks );

      const Function & function() const { return function_; }
      const std::set<std::string> & data_values() const { return data_.values; }
      const std::set<std::string> & data_thunks() const { return data_.thunks; }
      const std::set<std::string> & data_executables() const { return data_.executables; }
      const std::vector<std::string> & outputs() const { return outputs_; }

      gg::protobuf::Thunk to_protobuf() const;

      bool operator==( const Thunk & other ) const;
      bool operator!=( const Thunk & other ) const { return not operator==( other ); }

      void set_hash( const std::string & hash ) const { hash_.reset( hash ); }
      std::string hash() const;
      std::string executable_hash() const;
      bool executable() const { return ( data_.thunks.size() == 0 ); }
      size_t infiles_size( const bool include_executables = true ) const;

      void update_data( const std::string & old_hash,
                        const std::string & new_hash );

      /* Returns a list of files that can be accessed while executing this
         thunk. */
      std::unordered_map<std::string, Permissions> get_allowed_files() const;
    };

  } /* namespace thunk */
} /* namespace gg */

#endif /* THUNK_HH */
