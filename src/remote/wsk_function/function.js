#!/usr/bin/env node

var fs = require( 'fs' );
var path = require( 'path' );
var http = require( 'http' );
var https = require( 'https' );

var gg = require( './gg.js' );

function setup_environment( args )
{
  return new Promise( ( resolve, reject ) => {
    var gg_dir = '_gg';
    process.env[ 'PATH' ] = __dirname + ':' + process.env[ 'PATH' ];
    process.env[ 'GG_DIR' ] = path.resolve( gg_dir );

    fs.mkdir( gg_dir, 0o777, ( err ) => {
      if ( err && err.code !== 'EEXIST' ) {
        reject( err );
        return;
      }

      /* after the GG_DIR is set, we're ready to init gg module */
      gg.init();

      var thunk_data = Buffer.from( args[ 'thunk_data' ], 'base64' );
      fs.writeFileSync( gg.blob_path( args[ 'thunk_hash' ] ), thunk_data );

      resolve( args );
    } );
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
      download_list.push( infile[ 'hash' ] );
    }
  } );

  console.log( download_list );

  var donwload_promises = [];

  return Promise.all( download_list.map( ( hash ) => {
    return new Promise( ( resolve, reject ) => {
      var file = fs.createWriteStream( gg.blob_path( hash ) );
      var request = https.get( gg.object_url( args.s3_bucket, hash ), ( response ) => {
        response.pipe( file );
        response.on( 'end', () => { resolve( hash ) } );
      } );
    } )
  } ) );
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
    } );
}

exports.main = handler;

if ( require.main == module ) {
  handler( {
    'thunk_hash': '000000',
    'thunk_data': 'VGhlIEJhc2U2NCB0ZXJtIG9yaWdpbmF0ZXMgZnJvbSBhIHNwZWNpZmljIE1JTUUgY29udGVudCB0cmFuc2ZlciBlbmNvZGluZy4NCg==',
    's3_bucket': 'gg-us-west-2',
    's3_region': 'us-west-2',
    'infiles': [{
      'hash': '1_MXS0ArvAN5mUCDO9iKXhucGVlSUr3FgtFOwpUkvpY000231b2'
    }, {
      'hash': 'sdV7jt1YHfI__XzwM7elEM2IAjtMybcEcBuyq4NtsuM0000fbb0'
    }, {
      'hash': 'X1yp63ZBmUy_eUafGcsa7h4TafsbHQql4KK91r060.U000315c4'
    }]
  } );
}
