var AWS = require( 'aws-sdk' );
var KV_Store = require( 'kv-store' ).KV_Store;

function download_all( args )
{
  return Promise.all( args.files.map( ( file ) => {
    return Promise.resolve()
      .then( () => {
        return args.kvstore.get( file[ 'key' ] );
      } )
      .then( ( data ) => {
        var s3 = new AWS.S3();
        s3.upload( {
          'Bucket': args.s3_bucket,
          'Key': file.key,
          'Body': data
        }, ( err, data ) => {
          if ( err ) {
            return Promise.reject( err );
          }
        } );
      } );
  } ) );
}

function handler( args )
{
  args.kvstore = new KV_Store( args.kkv_host, args.kkv_username,
                               args.kkv_password );

  AWS.config.accessKeyId = args.aws_access_key;
  AWS.config.secretAccessKey = args.aws_secret_key;

  return Promise.resolve()
    .then( () => args.kvstore.init() )
    .then( () => {
      return download_all( args );
    } )
    .then( () => args.kvstore.close() )
    .then( () => ( { 'status': 'done' } ) );
}

exports.main = handler;

if ( require.main == module ) {
  var args = {
    'kkv_host': process.env[ 'KKV_HOST' ],
    'kkv_username': process.env[ 'KKV_USERNAME' ],
    'kkv_password': process.env[ 'KKV_PASSWORD' ],
    'aws_access_key': process.env[ 'AWS_ACCESS_KEY_ID' ],
    'aws_secret_key': process.env[ 'AWS_SECRET_ACCESS_KEY' ],
    's3_bucket': 'ggfunbucket',
    'files': [
      {
        'key': 'X12KCNT9qbus7T6RQ8mPc1viVAKMvVg7JNDkCP6Bk5k0018b1c0',
      },
      {
        'key': 'YBDbiCDCbDtlOUlrI_mIqWG2nxmWKpGNtssfSOxymZA0008ac5f',
      }
    ]
  };

  handler( args )
    .then( () => {
      console.log( 'success.' );
    }, ( err ) => {
      console.log( 'error: ' + err );
    } );
}
