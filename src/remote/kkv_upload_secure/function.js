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
                               args.kkv_password, true );

    return Promise.resolve()
    .then( () => args.kvstore.init())
    .then( () => {
      return upload_all( args );
    } )
    .then( () => args.kvstore.close() )
    .then( () => ( { 'status': 'done' } ) )
    .catch( ( reason ) => ( { 'status': 'error' } ) );

}

module.exports.main = handler;

