var fs = require( 'fs' );
var path = require( 'path' );
var child_process = require( 'child_process' );
var KV_Store = require( 'kv-store' ).KV_Store;

var gg = require( './gg.js' );

function setup_environment( args )
{
  return new Promise( ( resolve, reject ) => {
    var gg_dir = '_gg';

    args.execute_env = {
      'GG_DIR': path.resolve( path.join( __dirname, gg_dir ) ),
      'GG_SANDBOXED': '1',
    };

    args.gg_execute_path = path.join( __dirname, 'gg-execute-static' );

    console.log( args ); 
    console.log( __dirname ); 

    gg.make_executable( args.gg_execute_path );

    console.log( 'chomd done!' );

    fs.mkdir( gg_dir, 0o777, ( err ) => {
      if ( err && err.code !== 'EEXIST' ) {
        return reject( err );
      }

      /* after the GG_DIR is set, we're ready to init gg module */
      gg.init( args.execute_env.GG_DIR );

      console.log( 'gg module initialized' ); 

      var thunk_data = Buffer.from( args[ 'thunk_data' ], 'base64' );
      fs.writeFileSync( gg.blob_path( args[ 'thunk_hash' ] ), thunk_data );

      console.log( 'thunk is written to the disk. ');

      cleanup( args );

      console.log( 'cleanup is done.' );

      resolve( args );

    } );
  } );
}

function cleanup( args )
{
  var hashes = new Set( args[ 'infiles' ].map( ( infile ) => { return infile[ 'hash' ]; } ) );
  hashes.add( args.thunk_hash );

  var blobs_dir = gg.get_blobs_dir();
  var files = fs.readdirSync( blobs_dir );

  files.forEach( ( file ) => {
    if ( !hashes.has( file ) ) {
      fs.unlinkSync( path.join( blobs_dir, file ) );
    }
  } );
}

function fetch_dependencies( args )
{
  var download_list = [];

  args[ 'infiles' ].forEach( ( infile ) => {
    try {
      fs.accessSync( gg.blob_path( infile['hash'] ) );
    }
    catch ( err ) {
      download_list.push( infile );
    }
  } );

  var donwload_promises = [];

  return Promise.all( download_list.map( ( infile ) => {
    return Promise.resolve()
      .then( () => args.kvstore.get( infile.hash ) )
      .then( ( data ) => {
        return new Promise( ( resolve, reject ) => {
          fs.writeFile( gg.blob_path( infile.hash ), data, ( err ) => {
            if ( err ) {
              reject( err );
              return;
            }

            if ( infile.executable ) {
              gg.make_executable( gg.blob_path( infile.hash ) );
            }

            const stats = fs.statSync( gg.blob_path( infile.hash ) );
            if ( stats.size != infile.size ) {
              reject( 'infile size mismatch: ' + infile.hash );
              return;
            }

            resolve( infile.hash );
          } );
        } );
      } );
  } ) );
}

function execute_thunk( args )
{
  return new Promise( ( resolve, reject ) => {
    child_process.execSync( args.gg_execute_path + ' ' + args.thunk_hash, {
      'env': args.execute_env
    } );
    var output_hash = gg.check_cache( args.thunk_hash );

    if ( !output_hash ) {
      console.log( 'output_hash: |' + output_hash + '|' );
      throw new Error( 'GG-ExecutionFailed' );
    }

    args.output_hash = output_hash;
    resolve( args );
  } );
}

function upload_output( args )
{
  return Promise.resolve()
    .then( () => {
      return new Promise( ( resolve, reject ) => {
        fs.readFile( gg.blob_path( args.output_hash ), ( err, data ) => {
          if ( err ) {
            reject( err );
          }
          else {
            resolve( data );
          }
        } )
      } );
    } )
    .then( ( data ) => args.kvstore.put( args.output_hash, data ) );
}

function handler( args )
{
  console.log( 'gg handler function started.' );

  [ 'thunk_data', 'thunk_hash',
    'infiles', 'kkv_host', 'kkv_username',
    'kkv_password' ].forEach( ( element ) => {
    if ( !( element in args ) ) {
      throw new Error( 'argument "' + element + '" is missing' );
    }
  } );

  return setup_environment( args )
    .then( ( result ) => {
      console.log( 'connecting to k-v store...' )
      args.kvstore = new KV_Store( args.kkv_host, args.kkv_username, args.kkv_password, true );
      return args.kvstore.init();
    } )
    .then( ( result ) => {
      console.log( 'fetching dependencies...' );
      return fetch_dependencies( args );
    } )
    .then( ( result ) => {
      console.log( 'executing the thunk...' );
      return execute_thunk( args );
    } )
    .then( ( result ) => {
      console.log( 'output: ' + args.output_hash );
      return upload_output( args );
    } )
    .then( () => args.kvstore.close() )
    .then( result => ( {
      'thunk_hash': args.thunk_hash,
      'output_hash': args.output_hash,
      'output_size': fs.statSync( gg.blob_path( args.output_hash ) ).size,
      'executable_output': false,
    } ) )
    .catch( reason => {
      args.kvstore.close();
      return { 'error': reason };
    } );
}

exports.main = handler;

