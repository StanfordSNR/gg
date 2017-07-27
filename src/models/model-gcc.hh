/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef MODEL_LINKER_HH
#define MODEL_LINKER_HH

#include <string>
#include <vector>
#include <unordered_map>

#include "thunk.hh"
#include "toolchain.hh"

enum GCCStage
{
  PREPROCESS = 1,
  COMPILE,
  ASSEMBLE,
  LINK
};

enum class Language
{
  NONE,
  C,
  C_HEADER,
  CPP_OUTPUT,
  ASSEMBLER,
  OBJECT,
  ARCHIVE_LIBRARY,
  SHARED_LIBRARY,
};

struct InputFile
{
  std::string name;
  Language language;
  Language source_language;
  size_t index;
};

static const std::string GCC = "gcc";
static const std::string AS  = "as";
static const std::string CC1 = "cc1";
static const std::string COLLECT2 = "collect2";
static const std::string LD = "ld";
static const std::string GG_BIN_PREFIX = "/usr/bin/";
static const roost::path toolchain_path { std::string( TOOLCHAIN_PATH ) };

static auto gcc_function =
  []( const std::vector<std::string> & args,
      const std::vector<std::string> & envars ) -> gg::thunk::Function
  {
    return { GG_BIN_PREFIX + GCC, args, envars, program_hash( GCC ) };
  };

static const std::unordered_map<std::string, gg::thunk::InFile> program_infiles {
  {
    GCC,
    { GG_BIN_PREFIX + GCC, ( toolchain_path / GCC ).string(), program_hash( GCC ), 0 }
  },
  {
    CC1,
    { GG_BIN_PREFIX + CC1, ( toolchain_path / CC1 ).string(), program_hash( CC1 ), 0 }
  },
  {
    AS,
    { GG_BIN_PREFIX + AS, ( toolchain_path / AS ).string(), program_hash( AS ), 0 }
  },
  {
    COLLECT2,
    { GG_BIN_PREFIX + COLLECT2, ( toolchain_path / COLLECT2 ).string(), program_hash( COLLECT2 ), 0 }
  },
  {
    LD,
    { GG_BIN_PREFIX + LD, ( toolchain_path / LD ).string(), program_hash( LD ), 0 }
  },
};

bool is_non_object_input( const InputFile & input );

std::vector<std::string> get_link_dependencies( const std::vector<InputFile> & link_inputs,
                                                const std::vector<std::string> & args );

#endif /* MODEL_LINKER_HH */
