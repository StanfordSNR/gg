#!/usr/bin/env node

var fs = require( 'fs' );
var readline = require( 'readline' );
var KV_Store = require( 'kv-store' ).KV_Store;

[ 'KKV_HOST', 'KKV_USERNAME', 'KKV_PASSWORD' ].forEach( ( envar ) => {
  if ( !process.env[ envar ]  ) {
    throw new Error( envar + ' environment variable not found.' );
  }
} );

var kkv_host = process.env[ 'KKV_HOST' ];
var kkv_username = process.env[ 'KKV_USERNAME' ];
var kkv_password = process.env[ 'KKV_PASSWORD' ];

var kvstore = new KV_Store( kkv_host, kkv_username, kkv_password );

function download_files( file_list )
{
  process.stdout.write( 'downloading ' + file_list.length + ' file(s)...' );

  return Promise.all( file_list.map( ( entry ) => {
    return Promise.resolve()
      .then( () => kvstore.get( entry.key ) )
      .then( ( data ) => {
        return new Promise( ( resolve, reject ) => {
          fs.writeFile( entry.file_path, data, ( err ) => {
            if ( err ) {
              reject( err );
              return;
            }
            resolve()
          } );
        } );
      } );
  } ) );
}

Promise.resolve()
  .then( () => kvstore.init() )
  .then( () => {
    var rl = readline.createInterface({
      input: process.stdin,
      output: process.stdout,
      terminal: false
    });

    var files = []

    rl.on( 'line', ( input ) => {
      console.log( 'received: ' + input );

      var data = input.trim().split( ' ' );

      if ( data.length != 2 ) {
        return;
      }

      files.push( {
        'key': data[ 0 ],
        'file_path': data[ 1 ]
      } );
    } );

    return new Promise( ( resolve, reject ) => {
      rl.on( 'close', () => {
        download_files( files )
          .then( () => {
            resolve();
          } );
      } );
    } );
  } )
  .then( () => kvstore.close() )
  .catch( ( reason ) => {
    console.log( reason );
    kvstore.close();
    process.exit( 1 );
  } );
