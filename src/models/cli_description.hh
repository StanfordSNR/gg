/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef CLI_DESCRIPTION_HH
#define CLI_DESCRIPTION_HH

#include <string>
#include <vector>
#include <limits>

#include "util/optional.hh"

/* See also: http://docopt.org/ */

/* An example for a description string:
   path-to-static-bins/vpxenc -i,--input=@infile @outfile
*/

struct CLIOption
{
  int value { 0 };

  Optional<char> short_opt {};
  Optional<std::string> long_opt {};

  enum class Type { DoNotCare, InFile, OutFile } type { Type::DoNotCare };
};

class CLIDescription
{
private:
  std::string target_bin_ {};
  std::vector<CLIOption> options_ {};
  std::vector<size_t> infile_args_ {};
  std::vector<size_t> outfile_args_ {};

public:
  CLIDescription( const std::string & description );

  const std::string & target_bin() const { return target_bin_; }
  const std::vector<CLIOption> & options() const { return options_; }
  const std::vector<size_t> & infile_args() const { return infile_args_; }
  const std::vector<size_t> & outfile_args() const { return outfile_args_; }
};

#endif /* CLI_DESCRIPTION_HH */
