/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

/* NOTE
   Not really a model -- if the input to the nm tool is a thunk, it forces it
   and then executes the normal nm */

#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "thunk/placeholder.hh"
#include "thunk/ggutils.hh"
#include "util/path.hh"
#include "util/file_descriptor.hh"
#include "util/system_runner.hh"

#include "toolchain.hh"

using namespace std;
using namespace gg::thunk;

int main( int argc, char * argv[] )
{
  if ( argc < 2 ) {
    throw runtime_error( "not enough arguments" );
  }

  gg::models::init();

  for ( int i = 1; i < argc; i++ ) {
    if ( argv[ i ][ 0 ] == '-' ) {
      continue;
    }

    string path { argv[ i ] };

    struct stat stat_buf;
    const int stat_ret = stat( path.c_str(), &stat_buf );
    if ( stat_ret == 0 ) {
      /* (1) it exists -- is it a regular file? */
      if ( S_ISREG( stat_buf.st_mode ) ) {
        /* (2) it's a regular file -- can we open it? */
        const int fd_num = open( path.c_str(), O_RDONLY );
        if ( fd_num >= 0 ) {
          /* (3) successfully opened -- is it a thunk placeholder? */
          if ( ThunkPlaceholder::is_placeholder( FileDescriptor { fd_num } ) ) {
            /* (4) it's a thunk placeholder! let's force it */
            run( "gg-force", { "gg-force", path }, {}, true, true );
          }
        }
      }
    }
  }

  execvpe( "nm", argv, environ );

  return 0;
}
