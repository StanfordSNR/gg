/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef THUNK_HH
#define THUNK_HH

#include <string>
#include <vector>
#include <functional>
#include <map>
#include <unordered_map>
#include <limits>
#include <regex>
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
    Thunk = 'T',
  };

  namespace thunk {

    const std::string MAGIC_NUMBER = "##GGTHUNK##";
    const std::string BEGIN_REPLACE = "__GG_BEGIN_REPLACE__";
    const std::string END_REPLACE = "__GG_END_REPLACE__";

    const std::string DATA_PLACEHOLDER_START = "@{GGHASH:";
    const std::string DATA_PLACEHOLDER_END = "}";
    const std::regex DATA_PLACEHOLDER_REGEX { R"X(@\{GGHASH:([a-zA-Z0-9_.]+)\})X" };

    std::string data_placeholder( const std::string & hash );

    class Function
    {
    private:
      std::string hash_ {};
      std::vector<std::string> args_;
      std::vector<std::string> envars_ {};

    public:
      Function( const std::string & hash,
                const std::vector<std::string> & args,
                const std::vector<std::string> & envars );

      Function( const gg::protobuf::Function & func_proto );

      const std::string & hash() const { return hash_; }
      const std::vector<std::string> & args() const { return args_; }
      const std::vector<std::string> & envars() const { return envars_; }

      std::vector<std::string> & args() { return args_; }
      std::vector<std::string> & envars() { return envars_; }

      gg::protobuf::Function to_protobuf() const;

      bool operator==( const Function & other ) const;
      bool operator!=( const Function & other ) const { return not operator==( other ); }
    };

    class Thunk
    {
    public:
      typedef std::multimap<std::string, std::string> DataList;
      typedef DataList::value_type DataItem;

    private:
      Function function_;
      DataList values_;
      DataList thunks_;
      DataList executables_;
      std::vector<std::string> outputs_;

      mutable Optional<std::string> hash_ {};

      void throw_if_error() const;

    public:
      Thunk( const Function & function,
             const std::vector<DataItem> & data,
             const std::vector<DataItem> & executables,
             const std::vector<std::string> & outputs );

     Thunk( Function && function,
            std::vector<DataItem> && data,
            std::vector<DataItem> && executables,
            std::vector<std::string> && outputs );

      Thunk( const gg::protobuf::Thunk & thunk_proto );

      int execute() const;

      static std::string execution_payload( const Thunk & thunk );
      static std::string execution_payload( const std::vector<Thunk> & thunks );

      const Function & function() const { return function_; }
      const DataList & values() const { return values_; }
      const DataList & thunks() const { return thunks_; }
      const DataList & executables() const { return executables_; }
      const std::vector<std::string> & outputs() const { return outputs_; }

      gg::protobuf::Thunk to_protobuf() const;

      bool operator==( const Thunk & other ) const;
      bool operator!=( const Thunk & other ) const { return not operator==( other ); }

      void set_hash( const std::string & hash ) const { hash_.reset( hash ); }
      std::string hash() const;
      std::string executable_hash() const;
      std::string output_hash( const std::string & tag ) const;

      bool can_be_executed() const { return ( thunks_.size() == 0 ); }
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
