/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>

#include "thunk/thunk.hh"
#include "thunk/thunk_writer.hh"
#include "thunk/placeholder.hh"
#include "util/exception.hh"
#include "util/path.hh"
#include "util/util.hh"

using namespace std;
using namespace gg;
using namespace gg::thunk;

int num_out = 25;

int main( int argc, char * argv[] )
{
  try {
    if ( argc < 6 ) {
      cerr << argc << endl;
      cerr << "Usage: gen_vid_thunks25 <ffmpeg-hash> <listatic-hash> <.pb hash>";
      cerr << " <labels-hash> <vid-chunk1-hash> [<vid-chunk2-hash> ...]" << endl;
      return EXIT_FAILURE;
    }

    string ffmpeg_hash = argv[1];
    string listatic_hash = argv[2];
    string incept_hash = argv[3];
    string inet_hash = argv[4];

    int all_count = 0;
    for ( int i = 5; i < argc; ++i ) {
      string next_vid = argv[i];
      vector<string> all_outname;
      for ( int j = 0; j < num_out; ++j ) {
        char next_out[50];
        sprintf( next_out, "frameout%03d_%03d.jpg", j + 1, all_count );
        all_outname.push_back( { next_out } );
      }
      char ffmpeg_out[50];
      sprintf( ffmpeg_out, "frameout%%03d_%03d.jpg", all_count );
      vector<string> ffmpeg_args = { "ffmpeg", "-loglevel", "panic", "-i",
                                     thunk::data_placeholder( next_vid ),
                                     "-frames:v", to_string( num_out ),
                                     { ffmpeg_out } };

      const Thunk next_ffmpeg_thunk {
        { ffmpeg_hash, ffmpeg_args, {} },
        { { next_vid, "" } },
        { { ffmpeg_hash, "" } },
        { all_outname }
      };

      const string ffmpeg_thunk_hash = ThunkWriter::write( next_ffmpeg_thunk );

      char pic_out[50];
      sprintf( pic_out, "frameout%03d_lab.out", all_count );
      vector<string>listatic_args = { "li25-static" };
      vector<pair<const string, string>>listatic_values;
      for ( int j = 0; j < num_out; ++j ) {
        string t_name = ( j == 0 ) ? ffmpeg_thunk_hash
                                   : ( ffmpeg_thunk_hash + "#" + all_outname[j] );
        listatic_args.push_back( thunk::data_placeholder( t_name ) );
        listatic_values.push_back( make_pair( t_name, "" ) );
      }

      listatic_args.push_back( thunk::data_placeholder( incept_hash ) );
      listatic_values.push_back( make_pair( incept_hash, "" ) );
      listatic_args.push_back( thunk::data_placeholder( inet_hash ) );
      listatic_values.push_back( make_pair( inet_hash, "" ) );
      listatic_args.push_back( { pic_out } );

      const Thunk next_listatic_thunk {
        { listatic_hash, listatic_args, {} },
        listatic_values,
        { { listatic_hash, "" } },
        { { pic_out } }
      };

      const string listatic_thunk_hash = ThunkWriter::write( next_listatic_thunk );

      ThunkPlaceholder placeholder { listatic_thunk_hash };
      placeholder.write( { pic_out } );

      all_count++;
    }
  }
  catch ( const exception &  e ) {
    print_exception( argv[ 0 ], e );
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
