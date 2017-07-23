/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef MODEL_LINKER_HH
#define MODEL_LINKER_HH

#include <string>
#include <vector>

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

bool is_non_object_input( const InputFile & input );

std::vector<std::string> get_link_dependencies( const std::vector<InputFile> & link_inputs,
                                                const std::vector<std::string> & args );

#endif /* MODEL_LINKER_HH */
