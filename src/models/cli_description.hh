/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef CLI_DESCRIPTION_HH
#define CLI_DESCRIPTION_HH

#include <string>
#include <vector>
#include <limits>

/* See also: http://docopt.org/ */

/* An example for a description string:
   path-to-static-bins/vpxenc -i,--input=@infile @outfile
*/

struct CLIOption
{
  char short_opt { '\0' };
  std::string long_opt { "" };
  bool outfile { false };
};

class CLIDescription
{
private:
  std::string target_bin_ {};
  std::vector<CLIOption> options_ {};
  std::vector<size_t> infile_args_ {};
  size_t outfile_arg_ { std::numeric_limits<size_t>::max() };

public:
  CLIDescription( const std::string & description );
};

#endif /* CLI_DESCRIPTION_HH */
