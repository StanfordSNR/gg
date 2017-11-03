#!/usr/bin/env node

var fs = require( 'fs' );
var path = require( 'path' );
var http = require( 'http' );
var https = require( 'https' );
var child_process = require( 'child_process' );
var AWS = require( 'aws-sdk' );

var gg = require( './gg.js' );

function setup_environment( args )
{
  return new Promise( ( resolve, reject ) => {
    var gg_dir = '_gg';
    process.env[ 'PATH' ] = __dirname + ':' + process.env[ 'PATH' ];
    process.env[ 'GG_DIR' ] = path.resolve( gg_dir );

    gg.make_executable( path.join( __dirname, 'gg-execute-static' ) );

    fs.mkdir( gg_dir, 0o777, ( err ) => {
      if ( err && err.code !== 'EEXIST' ) {
        reject( err );
        return;
      }

      /* after the GG_DIR is set, we're ready to init gg module */
      gg.init();

      var thunk_data = Buffer.from( args[ 'thunk_data' ], 'base64' );
      fs.writeFileSync( gg.blob_path( args[ 'thunk_hash' ] ), thunk_data );

      cleanup( args );
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
    return new Promise( ( resolve, reject ) => {
      var file = fs.createWriteStream( gg.blob_path( infile.hash ) );
      var request = https.get( gg.object_url( args.s3_bucket, infile.hash ), ( response ) => {
        response.pipe( file );

        file.on( 'finish', () => {
          file.close();

          if ( infile.executable ) {
            gg.make_executable( gg.blob_path( infile.hash ) );
          }

          const stats = fs.statSync( gg.blob_path( infile.hash ) );
          if ( stats.size != infile.size ) {
            throw new Error( 'infile size mismatch: ' + infile.hash );
          }

          resolve( infile.hash )
        } ).on( 'error', ( e ) => { throw err; } );
      } );
    } )
  } ) );
}

function execute_thunk( args )
{
  return new Promise( ( resolve, reject ) => {
    child_process.execSync( 'gg-execute-static ' + args.thunk_hash );
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
  return new Promise( ( resolve, reject ) => {
    var outfile = fs.createReadStream( gg.blob_path( args.output_hash ) );

    var s3 = new AWS.S3();
    s3.upload( {
      'Bucket': args.s3_bucket,
      'Key': args.output_hash,
      'Body': outfile
    }, ( err, data ) => {
      if ( err ) {
        throw new Error( 'outfile upload failed' );
      }

      resolve( args );
    } );
  } );
}

function handler( args )
{
  console.log( 'gg handler function started.' );

  [ 'thunk_data', 'thunk_hash', 's3_bucket', 's3_region',
    'infiles' ].forEach( ( element ) => {
    if ( !( element in args ) ) {
      throw new Error( 'argument "' + element + '" is missing' );
    }
  } );

  setup_environment( args )
    .then( ( result ) => {
      return fetch_dependencies( args );
    } )
    .then( ( result ) => {
      return execute_thunk( args );
    } )
    .then( ( result ) => {
      console.log( 'output: ' + args.output_hash );
      return upload_output( args );
    } );
}

exports.main = handler;
