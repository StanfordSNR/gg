#!/usr/bin/env node

var https = require( 'https' );
var KV_Store = require( 'kv-store' ).KV_Store;

function upload_all( args )
{
  return Promise.all( args.files.map( ( file ) => {
    return new Promise( ( resolve, reject ) => {
      let raw_data = [];

      https.get( file[ 'url' ], ( response ) => {
        response.on( 'data', ( chunk ) => {
          raw_data.push( chunk );
        } );

        response.on( 'end', () => {
          resolve( args.kvstore.put( file[ 'key' ], Buffer.concat( raw_data ) ) );
        } );
      } );
    } )
  } ) );
}

function handler( args )
{
  args.kvstore = new KV_Store( args.kkv_host, args.kkv_username,
                               args.kkv_password );

  return Promise.resolve()
    .then( () => {
      return upload_all( args );
    } )
    .then( () => args.kvstore.close() )
    .then( () => ( { 'status': 'done' } ) )
    .catch( ( reason ) => ( { 'status': 'error' } ) );

}

exports.main = handler;

if ( require.main == module ) {
  var args = {
    'kkv_host': process.env[ 'KKV_HOST' ],
    'kkv_username': process.env[ 'KKV_USERNAME' ],
    'kkv_password': process.env[ 'KKV_PASSWORD' ],
    'files': [
      {
        'key': 'X12KCNT9qbus7T6RQ8mPc1viVAKMvVg7JNDkCP6Bk5k0018b1c0',
        'url': 'https://s3-us-west-2.amazonaws.com/gg-us-west-2/X12KCNT9qbus7T6RQ8mPc1viVAKMvVg7JNDkCP6Bk5k0018b1c0',
      },
      {
        'key': 'YBDbiCDCbDtlOUlrI_mIqWG2nxmWKpGNtssfSOxymZA0008ac5f',
        'url': 'https://s3-us-west-2.amazonaws.com/gg-us-west-2/YBDbiCDCbDtlOUlrI_mIqWG2nxmWKpGNtssfSOxymZA0008ac5f',
      }
    ]
  };

  console.log( JSON.stringify( args ) );
  return;

  handler( args )
    .then( () => {
      console.log( 'success.' );
    }, () => {
      console.log( 'error.' );
    } );
}
