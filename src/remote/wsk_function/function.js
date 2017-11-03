#!/usr/bin/env node

var fs = require( 'fs' );
var path = require( 'path' );
var http = require( 'http' );
var https = require( 'https' );
var child_process = require( 'child_process' );

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

        response.on( 'finish', () => {
          if ( infile.executable ) {
            gg.make_executable( gg.blob_path( infile.hash ) );
          }
          file.close();

          const stats = fs.statSync( gg.blob_path( infile.hash ) );

          if ( stats.size != infile.size ) {
            console.log( stats.size );
            console.log( infile.size );
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

    } );
}

exports.main = handler;

if ( require.main == module ) {
  handler( {
	"thunk_hash" : "En_yRtN_oWslLjEG869c1OSsO82rRDL5CEBpRJxJMxg0000195d",
	"s3_bucket" : "gg-us-west-2",
	"s3_region" : "us-west-2",
	"thunk_data" : "IyNHR1RIVU5LIyMK6QYKCy9fX2dnX18vZysrEhgtc3BlY3M9L19fZ2dfXy9nY2Mtc3BlY3MSCS1ub3N0ZGluYxIaLWlzeXN0ZW0vdXNyL2luY2x1ZGUvYysrLzcSKy1pc3lzdGVtL3Vzci9pbmNsdWRlL3g4Nl82NC1saW51eC1nbnUvYysrLzcSIy1pc3lzdGVtL3Vzci9pbmNsdWRlL2MrKy83L2JhY2t3YXJkEi8taXN5c3RlbS91c3IvbGliL2djYy94ODZfNjQtbGludXgtZ251LzcvaW5jbHVkZRIaLWlzeXN0ZW0vdXNyL2xvY2FsL2luY2x1ZGUSNS1pc3lzdGVtL3Vzci9saWIvZ2NjL3g4Nl82NC1saW51eC1nbnUvNy9pbmNsdWRlLWZpeGVkEiUtaXN5c3RlbS91c3IvaW5jbHVkZS94ODZfNjQtbGludXgtZ251EhQtaXN5c3RlbS91c3IvaW5jbHVkZRILLW5vc3RkaW5jKysSDy1ESEFWRV9DT05GSUdfSBIDLUkuEgctSS4uLy4uEgstSS4vLi4vdXRpbBIFLVdhbGwSFC1mbm8tc3RyaWN0LW92ZXJmbG93EhMtRF9GT1JUSUZZX1NPVVJDRT0yEhUtZnN0YWNrLXByb3RlY3Rvci1hbGwSES1Xc3RhY2stcHJvdGVjdG9yEgctLXBhcmFtEhFzc3AtYnVmZmVyLXNpemU9MRIFLWZQSUUSEy1mbm8tZGVmYXVsdC1pbmxpbmUSBS1waXBlEgItZxIDLU8yEgIteBIDYysrEgZvY2IuY2MSAi1FEjotZnJhbmRvbS1zZWVkPV9fR0dfQkVHSU5fUkVQTEFDRV9fb2NiLmNjX19HR19FTkRfUkVQTEFDRV9fEhwtV25vLWJ1aWx0aW4tbWFjcm8tcmVkZWZpbmVkEhotRF9fVElNRVNUQU1QX189IlJFREFDVEVEIhIVLURfX0RBVEVfXz0iUkVEQUNURUQiEhUtRF9fVElNRV9fPSJSRURBQ1RFRCISHS1mbm8tY2Fub25pY2FsLXN5c3RlbS1oZWFkZXJzEgItbxIKb3V0cHV0XzBfMRozc0oucms1NWVrV2FrRVZjSUtZcklkc0dBQzhpdm8ua1J4NzVhNEc4Y3k0bzAwMTI0NTEwIgxQQVRIPS9fX2dnX18SQQoGb2NiLmNjEjN4d1QwcmF2VDNQRHRNTVExTUFuNHUzMVdmb05WMHlIS2VpWFlpUDVqbGhFMDAwMGQ0ZGYg36kDEkgKCy9fX2dnX18vZysrEjNzSi5yazU1ZWtXYWtFVmNJS1lySWRzR0FDOGl2by5rUng3NWE0RzhjeTRvMDAxMjQ1MTAgkIpJKAESSwoRL19fZ2dfXy9nY2Mtc3BlY3MSM1M0QmV4MnVsZjAuczVnbTkwMUR2aDNvUFlnMEdYR3l4akFnT1pGbHlLS0EwMDAwMjZiMyCzTRJNCg8vX19nZ19fL2NjMXBsdXMSMzFTS0xZM21NRjNRWHZtY2Zfb0dPTWI5VUJOTDdJYWZSVkEyX01JSUpENEkwMTdlYzlmOCD4k/sLKAESQQoGb2NiLmNjEjN4d1QwcmF2VDNQRHRNTVExTUFuNHUzMVdmb05WMHlIS2VpWFlpUDVqbGhFMDAwMGQ0ZGYg36kDEkgKDi4uLy4uL2NvbmZpZy5oEjNnbjkyM3ZSMXhFQWxCTVBPZXNPbG1hNkNyckd6TGguc3kudjNkS1o0QWtBMDAwMDJmYjMgs18SPgoEYWUuaBIzRnNDSUFyWkQ3NGNNZHVSbl9VWXhSeVJtRVVPMzViMnFqaVBnS2xld1dFRTAwMDAxZTBlII48ElUKGy91c3IvaW5jbHVkZS9jKysvNy9zdGRsaWIuaBIzZ3hPT0FoakxMVFR5bGlRTktXLlVkbXRGWGpJVGNxd2M5QmxwNjdWUnVRbzAwMDAwOGM4IMgRElQKGi91c3IvaW5jbHVkZS9jKysvNy9jc3RkbGliEjNCRlcwZ0plV09ndnI3QnN6SDloQ2V1bnN3UkRwbEEwS01xMFkwZUF5NUJjMDAwMDE3ZjIg8i8Sbwo0L3Vzci9pbmNsdWRlL3g4Nl82NC1saW51eC1nbnUvYysrLzcvYml0cy9jKytjb25maWcuaBIzaEU2RFM0WGpIU0p4ZVVwa21EVDNRQlR5ZmpxNWVTSm5VV2h0dl9jenBlbzAwMDBlMjZlIO7EAxJvCjUvdXNyL2luY2x1ZGUveDg2XzY0LWxpbnV4LWdudS9jKysvNy9iaXRzL29zX2RlZmluZXMuaBIzeGFsdXN4aWtSdG45Vk1RRnVabVBKTzZjMlhhTU1NdlNvUDVmekNJa1E3YzAwMDAwN2Q3INcPElEKFy91c3IvaW5jbHVkZS9mZWF0dXJlcy5oEjNkbndodVlLTl9DOGpZdUNNbHVjS3pEUUV2V3hjMUFjVGNyVHIyZTVOcFVjMDAwMDMzMGQgjWYSVAoaL3Vzci9pbmNsdWRlL3N0ZGMtcHJlZGVmLmgSM0pWdUtVYVhJZkY3T2MwdFJzWkZZLmVteThEeV9SeGVLMnRpX0dwSTgyRDAwMDAwMDk2MSDhEhJjCikvdXNyL2luY2x1ZGUveDg2XzY0LWxpbnV4LWdudS9zeXMvY2RlZnMuaBIzdXdod0tuTnhmcVJxRHZBNnIuZ05FYmVmZ3F4WFhYQVJadmRQcHFPN3lSQTAwMDAzZDIzIKN6EmcKLS91c3IvaW5jbHVkZS94ODZfNjQtbGludXgtZ251L2JpdHMvd29yZHNpemUuaBIzbW1DT0VSSW5rNUNiRlIyZDI5T0JzT2VyMndVRG5ycHppNmFjU3ltbnRRWTAwMDAwMTQ3IMcCEmMKKS91c3IvaW5jbHVkZS94ODZfNjQtbGludXgtZ251L2dudS9zdHVicy5oEjM3MFJDbGFDcDZQWEVDUmc1bWdETWtib0wxNWQwRVF6ZmJtVXJxbXo2Mnk0MDAwMDAxODAggAMSZgosL3Vzci9pbmNsdWRlL3g4Nl82NC1saW51eC1nbnUvZ251L3N0dWJzLTY0LmgSM3BmY2hsMXQxay5GZV9pejFYT1hOMVhjR25iUXpZZThPYWlzcjFpWUFBUUkwMDAwMDI2NSDlBBJwCjYvdXNyL2luY2x1ZGUveDg2XzY0LWxpbnV4LWdudS9jKysvNy9iaXRzL2NwdV9kZWZpbmVzLmgSM0llWkp5bF80eDVrVXZRVmNSWS5adUJudE9iTTJ4S0N4enVnOVZRNFlJUk0wMDAwMDUzNSC1ChJQChUvdXNyL2luY2x1ZGUvc3RkbGliLmgSM3VackV4NW5rUDhHX3NpcXg4RUZVeGhuVHpuUGhWU1NvalpJa29qQ19DeHcwMDAwN2Y5OCCY/wESagowL3Vzci9saWIvZ2NjL3g4Nl82NC1saW51eC1nbnUvNy9pbmNsdWRlL3N0ZGRlZi5oEjNIc2FONnNMRndQaEdsSjNtZllKbXdDbGtFeS5pRmhaZW91eE9jTWo4dVRBMDAwMDM3M2MgvG4SaAouL3Vzci9pbmNsdWRlL3g4Nl82NC1saW51eC1nbnUvYml0cy93YWl0ZmxhZ3MuaBIzcFNwZ0xXSm93Mm1oRnVYaTdjSnoyMlNDMjN6TGJrQy51SWhXWjcxMTl0STAwMDAwODVjINwQEmkKLy91c3IvaW5jbHVkZS94ODZfNjQtbGludXgtZ251L2JpdHMvd2FpdHN0YXR1cy5oEjMwOVpvOE9qZWoubDVMdEhtNVkwQkljVlNBNXpyNWNoaEMuUEd2ZlV4VFJjMDAwMDA4ZWUg7hESUAoWL3Vzci9pbmNsdWRlL3hsb2NhbGUuaBIzXzJaSVlKLlIxOFZEb3FjWjY0RjROYkpqcGF1SDMwclkwZW12aFBab21HTTAwMDAwNjk5IJkNEmMKKS91c3IvaW5jbHVkZS94ODZfNjQtbGludXgtZ251L3N5cy90eXBlcy5oEjNXSU4xMlZpeFVDVDQyUTZJR0ZGZ0JvVkdFa0tFM3hoLlJia09OZHdXSnk0MDAwMDFhMWYgnzQSZAoqL3Vzci9pbmNsdWRlL3g4Nl82NC1saW51eC1nbnUvYml0cy90eXBlcy5oEjNzRzBYclEuWkpQQmo2eldpdzdJc3FiZDFESWlPRzN6azNDZHpHcXUyLlUwMDAwMDFkOGIgizsSaAouL3Vzci9pbmNsdWRlL3g4Nl82NC1saW51eC1nbnUvYml0cy90eXBlc2l6ZXMuaBIzYXpZNVNHMmFYOWRBVTFDV2ppMlNLZUYxT09mUDRjQkxxQWFYanlGMlRLUTAwMDAwY2NjIMwZEk0KEy91c3IvaW5jbHVkZS90aW1lLmgSM2NESkIueHFhM0swYWZNb2xfQVhZeGN6MzBsUlJCc092U3VsRTFXRS4zNlUwMDAwMzRlNyDnaRJPChUvdXNyL2luY2x1ZGUvZW5kaWFuLmgSM3h0VHdOQ3dBS2t3YTRhV2ZZNU5BSlNVVk50Y2gyNi53NnJtTDdjTHFibmMwMDAwMGI3YiD7FhJlCisvdXNyL2luY2x1ZGUveDg2XzY0LWxpbnV4LWdudS9iaXRzL2VuZGlhbi5oEjNMQUtLQ042NkNZdGRPVFFHRl8uVDhWb0FwU1ZxRUx5T2I4bUNrYUswbEVvMDAwMDAwYjAgsAESZwotL3Vzci9pbmNsdWRlL3g4Nl82NC1saW51eC1nbnUvYml0cy9ieXRlc3dhcC5oEjNKWUxHZWFpYUQwbERhbnRVOV81eFU4ZTRFcmdfSW9Pc284OFdZVmlmQUdJMDAwMDE2MjAgoCwSagowL3Vzci9pbmNsdWRlL3g4Nl82NC1saW51eC1nbnUvYml0cy9ieXRlc3dhcC0xNi5oEjNlZjUyU3FrcTdaRlJ1RUo1VEdwWTNVS2kzQVl4UEFWN0sySG1yeUJpQ0JJMDAwMDA2YzMgww0SZAoqL3Vzci9pbmNsdWRlL3g4Nl82NC1saW51eC1nbnUvc3lzL3NlbGVjdC5oEjM4SnJCdlphUEdDSTdHS2ljaUpEZnVHcjVUal9fdHBiWWFUT0JOaThyRXk0MDAwMDEwODIggiESZQorL3Vzci9pbmNsdWRlL3g4Nl82NC1saW51eC1nbnUvYml0cy9zZWxlY3QuaBIzRDZOMUNXVkEzLk5UZ2hGdG5uaWJkbnJybERYc2xiSTZmYWdIWVU1Y2hqRTAwMDAwODM5ILkQEmUKKy91c3IvaW5jbHVkZS94ODZfNjQtbGludXgtZ251L2JpdHMvc2lnc2V0LmgSMy4uc3hsSVhGN29kNDdwT2hhZnBVMFdTbVZ2SmNHWVlaTlRTV2tIa2hESEEwMDAwMTFjOSDJIxJjCikvdXNyL2luY2x1ZGUveDg2XzY0LWxpbnV4LWdudS9iaXRzL3RpbWUuaBIzYUNGWjBld2RURHlHQjJ1aHllNEwwemZzYU8wdDF0YU5HVGozd1V0YXppWTAwMDAwZDg5IIkbEmYKLC91c3IvaW5jbHVkZS94ODZfNjQtbGludXgtZ251L2JpdHMvc2VsZWN0Mi5oEjNucFZUVHhTU0hHRUtOZ1dueE9PQmhGNzVkZEZ0dHJRRkNuNnA5YWNEYXBRMDAwMDA1OWQgnQsSZwotL3Vzci9pbmNsdWRlL3g4Nl82NC1saW51eC1nbnUvc3lzL3N5c21hY3Jvcy5oEjNqUmZvaVVGLloyMVI5aTg0Y1hoSWtKcVdHM2o4SWVIWWZnbXJVRWxGVV9ZMDAwMDA4ZTYg5hESawoxL3Vzci9pbmNsdWRlL3g4Nl82NC1saW51eC1nbnUvYml0cy9wdGhyZWFkdHlwZXMuaBIza3NtY1ZtUngybWp6eGZLZkJGYnNWVjFWNjA1aGFRbVZpblROU2lJWGhyQTAwMDAxYTU1INU0Ek8KFS91c3IvaW5jbHVkZS9hbGxvY2EuaBIzTGNSRmlwaktSeURvN21JRlc1a2NTNEJGRmZ4ZEVqVUpZTUYyVkhzWFFpazAwMDAwNGIzILMJEm0KMy91c3IvaW5jbHVkZS94ODZfNjQtbGludXgtZ251L2JpdHMvc3RkbGliLWJzZWFyY2guaBIzODNrd0lFNXE4QTk5NmxhZHJNa3kzY1Z4UGJZc2JKRnBLdGpiWTVQenNxVTAwMDAwNTYxIOEKEmsKMS91c3IvaW5jbHVkZS94ODZfNjQtbGludXgtZ251L2JpdHMvc3RkbGliLWZsb2F0LmgSM1VzSklLci5wWHRGTEFhTC5nWmdqUUJoOFgxMXJBN0I3czZvNTZ4eFlHMWswMDAwMDQ4NCCECRJlCisvdXNyL2luY2x1ZGUveDg2XzY0LWxpbnV4LWdudS9iaXRzL3N0ZGxpYi5oEjNCTTNUM1NleTJZRWhhYkplWWpkbFhCUVlacDN1NW9ndEV1aHMwSUJiSmk0MDAwMDE2MWEgmiwSWwohL3Vzci9pbmNsdWRlL2MrKy83L2JpdHMvc3RkX2Ficy5oEjNWQUZfMjROcFFWbnRXbTZBVzFmNmduZ2lQeXo3TllJZWdtd2NBQ3Awc1JZMDAwMDBjYzIgwhkSUAoVL3Vzci9pbmNsdWRlL3N0cmluZy5oEjNESWd3b3d2Yjk0VzhnWDBwa0p5Xzk0TDJNU2g1OTVpdlVnWW9OejJLNGlRMDAwMDU4NTkg2bABEmYKLC91c3IvaW5jbHVkZS94ODZfNjQtbGludXgtZ251L2JpdHMvc3RyaW5nMy5oEjNycks3OVF4d2xNRUU3Lkw2QmQwekhRMTdQTlZRNV9OenpTTksuVVRIZUFjMDAwMDEyMTcglyQSUAoWL3Vzci9pbmNsdWRlL3N0cmluZ3MuaBIzWXhkREx4TFZNVUs4VnQ1aDd3TW1Xcnp1NVZNNlV0YXNEUmtqRnhUVUVEUTAwMDAxMjA2IIYkEmoKMC91c3IvbGliL2djYy94ODZfNjQtbGludXgtZ251LzcvaW5jbHVkZS9zdGRpbnQuaBIzaHFlUlNyZldvWVJscHZ1YWUxVDVSY3JXQlAyX1BLRFJEazRDVk9BRGVSRTAwMDAwMTQ4IMgCEk8KFS91c3IvaW5jbHVkZS9zdGRpbnQuaBIzd1FOT2VXMFlQWHhzSm9QdGpfd25ncjc3dFFjT1hIVDJTbV9FT3R2T1c5NDAwMDAxZjllIJ4/EmQKKi91c3IvaW5jbHVkZS94ODZfNjQtbGludXgtZ251L2JpdHMvd2NoYXIuaBIzTGk3SHpMdTBTOVZWVkt5SUtBbkV5MWxCMVdjT1JfTWxTRXlJOUduXzBySTAwMDAwNzcxIPEOEm4KMy91c3IvbGliL2djYy94ODZfNjQtbGludXgtZ251LzcvaW5jbHVkZS94bW1pbnRyaW4uaBIzQWZuUTN5VWhVYlI5Z0EwZ1pFRzlRX3p5YVlpVTRwTzdDbURqRzVEM3YwYzAwMDBhNGUyIOLJAhJtCjIvdXNyL2xpYi9nY2MveDg2XzY0LWxpbnV4LWdudS83L2luY2x1ZGUvbW1pbnRyaW4uaBIzUmlKVVFFTm4uWEFHclE2Z2FyMFA2ZnlWWkFnRWg1QlFXbGxTWWt0RHVSWTAwMDA3YTdhIPr0ARJtCjMvdXNyL2xpYi9nY2MveDg2XzY0LWxpbnV4LWdudS83L2luY2x1ZGUvbW1fbWFsbG9jLmgSM0JMNWRRZHFwRENzTXJleDBzdkJ1UlZFRTNPRVVKNm8udDExM0E2WkZpbzgwMDAwMDZmNyD3DRJuCjMvdXNyL2xpYi9nY2MveDg2XzY0LWxpbnV4LWdudS83L2luY2x1ZGUvZW1taW50cmluLmgSM0R4N1ZGbXkuQUdURXhvOURESnRGYmhOaU1jZzROaHk5TUVRUGZfSzZDT2swMDAwYzcxNSCVjgMSVAoaL3Vzci9pbmNsdWRlL29wZW5zc2wvYWVzLmgSM2JzRHNCbEJMU2ZacDg1V2lqSklnZDhid3pyanNPQlJZT0FSUTZ2NXcwcG8wMDAwMTgwMiCCMBJtCjMvdXNyL2luY2x1ZGUveDg2XzY0LWxpbnV4LWdudS9vcGVuc3NsL29wZW5zc2xjb25mLmgSM3NxcDBzOGZ2VVJNUzVzN0lWZG5FMEtpMTIub1BmbWs0bHZxREtmQmZtemswMDAwMWNiNyC3ORIWChIvdXNyL2luY2x1ZGUvYysrLzcoAhInCiMvdXNyL2luY2x1ZGUveDg2XzY0LWxpbnV4LWdudS9jKysvNygCEh8KGy91c3IvaW5jbHVkZS9jKysvNy9iYWNrd2FyZCgCEisKJy91c3IvbGliL2djYy94ODZfNjQtbGludXgtZ251LzcvaW5jbHVkZSgCEhYKEi91c3IvbG9jYWwvaW5jbHVkZSgCEjEKLS91c3IvbGliL2djYy94ODZfNjQtbGludXgtZ251LzcvaW5jbHVkZS1maXhlZCgCEiEKHS91c3IvaW5jbHVkZS94ODZfNjQtbGludXgtZ251KAISEAoML3Vzci9pbmNsdWRlKAISAigCEgkKBS4uLy4uKAISCwoHLi4vdXRpbCgCEgIoAhoKb3V0cHV0XzBfMQ==",
	"infiles" : [
		{
			"hash" : "xwT0ravT3PDtMMQ1MAn4u31WfoNV0yHKeiXYiP5jlhE0000d4df",
			"size" : 54495,
			"executable" : false
		},
		{
			"hash" : "sJ.rk55ekWakEVcIKYrIdsGAC8ivo.kRx75a4G8cy4o00124510",
			"size" : 1197328,
			"executable" : true
		},
		{
			"hash" : "S4Bex2ulf0.s5gm901Dvh3oPYg0GXGyxjAgOZFlyKKA000026b3",
			"size" : 9907,
			"executable" : false
		},
		{
			"hash" : "1SKLY3mMF3QXvmcf_oGOMb9UBNL7IafRVA2_MIIJD4I017ec9f8",
			"size" : 25086456,
			"executable" : true
		},
		{
			"hash" : "xwT0ravT3PDtMMQ1MAn4u31WfoNV0yHKeiXYiP5jlhE0000d4df",
			"size" : 54495,
			"executable" : false
		},
		{
			"hash" : "gn923vR1xEAlBMPOesOlma6CrrGzLh.sy.v3dKZ4AkA00002fb3",
			"size" : 12211,
			"executable" : false
		},
		{
			"hash" : "FsCIArZD74cMduRn_UYxRyRmEUO35b2qjiPgKlewWEE00001e0e",
			"size" : 7694,
			"executable" : false
		},
		{
			"hash" : "gxOOAhjLLTTyliQNKW.UdmtFXjITcqwc9Blp67VRuQo000008c8",
			"size" : 2248,
			"executable" : false
		},
		{
			"hash" : "BFW0gJeWOgvr7BszH9hCeunswRDplA0KMq0Y0eAy5Bc000017f2",
			"size" : 6130,
			"executable" : false
		},
		{
			"hash" : "hE6DS4XjHSJxeUpkmDT3QBTyfjq5eSJnUWhtv_czpeo0000e26e",
			"size" : 57966,
			"executable" : false
		},
		{
			"hash" : "xalusxikRtn9VMQFuZmPJO6c2XaMMMvSoP5fzCIkQ7c000007d7",
			"size" : 2007,
			"executable" : false
		},
		{
			"hash" : "dnwhuYKN_C8jYuCMlucKzDQEvWxc1AcTcrTr2e5NpUc0000330d",
			"size" : 13069,
			"executable" : false
		},
		{
			"hash" : "JVuKUaXIfF7Oc0tRsZFY.emy8Dy_RxeK2ti_GpI82D000000961",
			"size" : 2401,
			"executable" : false
		},
		{
			"hash" : "uwhwKnNxfqRqDvA6r.gNEbefgqxXXXARZvdPpqO7yRA00003d23",
			"size" : 15651,
			"executable" : false
		},
		{
			"hash" : "mmCOERInk5CbFR2d29OBsOer2wUDnrpzi6acSymntQY00000147",
			"size" : 327,
			"executable" : false
		},
		{
			"hash" : "70RClaCp6PXECRg5mgDMkboL15d0EQzfbmUrqmz62y400000180",
			"size" : 384,
			"executable" : false
		},
		{
			"hash" : "pfchl1t1k.Fe_iz1XOXN1XcGnbQzYe8Oaisr1iYAAQI00000265",
			"size" : 613,
			"executable" : false
		},
		{
			"hash" : "IeZJyl_4x5kUvQVcRY.ZuBntObM2xKCxzug9VQ4YIRM00000535",
			"size" : 1333,
			"executable" : false
		},
		{
			"hash" : "uZrEx5nkP8G_siqx8EFUxhnTznPhVSSojZIkojC_Cxw00007f98",
			"size" : 32664,
			"executable" : false
		},
		{
			"hash" : "HsaN6sLFwPhGlJ3mfYJmwClkEy.iFhZeouxOcMj8uTA0000373c",
			"size" : 14140,
			"executable" : false
		},
		{
			"hash" : "pSpgLWJow2mhFuXi7cJz22SC23zLbkC.uIhWZ7119tI0000085c",
			"size" : 2140,
			"executable" : false
		},
		{
			"hash" : "09Zo8Ojej.l5LtHm5Y0BIcVSA5zr5chhC.PGvfUxTRc000008ee",
			"size" : 2286,
			"executable" : false
		},
		{
			"hash" : "_2ZIYJ.R18VDoqcZ64F4NbJjpauH30rY0emvhPZomGM00000699",
			"size" : 1689,
			"executable" : false
		},
		{
			"hash" : "WIN12VixUCT42Q6IGFFgBoVGEkKE3xh.RbkONdwWJy400001a1f",
			"size" : 6687,
			"executable" : false
		},
		{
			"hash" : "sG0XrQ.ZJPBj6zWiw7Isqbd1DIiOG3zk3CdzGqu2.U000001d8b",
			"size" : 7563,
			"executable" : false
		},
		{
			"hash" : "azY5SG2aX9dAU1CWji2SKeF1OOfP4cBLqAaXjyF2TKQ00000ccc",
			"size" : 3276,
			"executable" : false
		},
		{
			"hash" : "cDJB.xqa3K0afMol_AXYxcz30lRRBsOvSulE1WE.36U000034e7",
			"size" : 13543,
			"executable" : false
		},
		{
			"hash" : "xtTwNCwAKkwa4aWfY5NAJSUVNtch26.w6rmL7cLqbnc00000b7b",
			"size" : 2939,
			"executable" : false
		},
		{
			"hash" : "LAKKCN66CYtdOTQGF_.T8VoApSVqELyOb8mCkaK0lEo000000b0",
			"size" : 176,
			"executable" : false
		},
		{
			"hash" : "JYLGeaiaD0lDantU9_5xU8e4Erg_IoOso88WYVifAGI00001620",
			"size" : 5664,
			"executable" : false
		},
		{
			"hash" : "ef52Sqkq7ZFRuEJ5TGpY3UKi3AYxPAV7K2HmryBiCBI000006c3",
			"size" : 1731,
			"executable" : false
		},
		{
			"hash" : "8JrBvZaPGCI7GKiciJDfuGr5Tj__tpbYaTOBNi8rEy400001082",
			"size" : 4226,
			"executable" : false
		},
		{
			"hash" : "D6N1CWVA3.NTghFtnnibdnrrlDXslbI6fagHYU5chjE00000839",
			"size" : 2105,
			"executable" : false
		},
		{
			"hash" : "..sxlIXF7od47pOhafpU0WSmVvJcGYYZNTSWkHkhDHA000011c9",
			"size" : 4553,
			"executable" : false
		},
		{
			"hash" : "aCFZ0ewdTDyGB2uhye4L0zfsaO0t1taNGTj3wUtaziY00000d89",
			"size" : 3465,
			"executable" : false
		},
		{
			"hash" : "npVTTxSSHGEKNgWnxOOBhF75ddFttrQFCn6p9acDapQ0000059d",
			"size" : 1437,
			"executable" : false
		},
		{
			"hash" : "jRfoiUF.Z21R9i84cXhIkJqWG3j8IeHYfgmrUElFU_Y000008e6",
			"size" : 2278,
			"executable" : false
		},
		{
			"hash" : "ksmcVmRx2mjzxfKfBFbsVV1V605haQmVinTNSiIXhrA00001a55",
			"size" : 6741,
			"executable" : false
		},
		{
			"hash" : "LcRFipjKRyDo7mIFW5kcS4BFFfxdEjUJYMF2VHsXQik000004b3",
			"size" : 1203,
			"executable" : false
		},
		{
			"hash" : "83kwIE5q8A996ladrMky3cVxPbYsbJFpKtjbY5PzsqU00000561",
			"size" : 1377,
			"executable" : false
		},
		{
			"hash" : "UsJIKr.pXtFLAaL.gZgjQBh8X11rA7B7s6o56xxYG1k00000484",
			"size" : 1156,
			"executable" : false
		},
		{
			"hash" : "BM3T3Sey2YEhabJeYjdlXBQYZp3u5ogtEuhs0IBbJi40000161a",
			"size" : 5658,
			"executable" : false
		},
		{
			"hash" : "VAF_24NpQVntWm6AW1f6gngiPyz7NYIegmwcACp0sRY00000cc2",
			"size" : 3266,
			"executable" : false
		},
		{
			"hash" : "DIgwowvb94W8gX0pkJy_94L2MSh595ivUgYoNz2K4iQ00005859",
			"size" : 22617,
			"executable" : false
		},
		{
			"hash" : "rrK79QxwlMEE7.L6Bd0zHQ17PNVQ5_NzzSNK.UTHeAc00001217",
			"size" : 4631,
			"executable" : false
		},
		{
			"hash" : "YxdDLxLVMUK8Vt5h7wMmWrzu5VM6UtasDRkjFxTUEDQ00001206",
			"size" : 4614,
			"executable" : false
		},
		{
			"hash" : "hqeRSrfWoYRlpvuae1T5RcrWBP2_PKDRDk4CVOADeRE00000148",
			"size" : 328,
			"executable" : false
		},
		{
			"hash" : "wQNOeW0YPXxsJoPtj_wngr77tQcOXHT2Sm_EOtvOW9400001f9e",
			"size" : 8094,
			"executable" : false
		},
		{
			"hash" : "Li7HzLu0S9VVVKyIKAnEy1lB1WcOR_MlSEyI9Gn_0rI00000771",
			"size" : 1905,
			"executable" : false
		},
		{
			"hash" : "AfnQ3yUhUbR9gA0gZEG9Q_zyaYiU4pO7CmDjG5D3v0c0000a4e2",
			"size" : 42210,
			"executable" : false
		},
		{
			"hash" : "RiJUQENn.XAGrQ6gar0P6fyVZAgEh5BQWllSYktDuRY00007a7a",
			"size" : 31354,
			"executable" : false
		},
		{
			"hash" : "BL5dQdqpDCsMrex0svBuRVEE3OEUJ6o.t113A6ZFio8000006f7",
			"size" : 1783,
			"executable" : false
		},
		{
			"hash" : "Dx7VFmy.AGTExo9DDJtFbhNiMcg4Nhy9MEQPf_K6COk0000c715",
			"size" : 50965,
			"executable" : false
		},
		{
			"hash" : "bsDsBlBLSfZp85WijJIgd8bwzrjsOBRYOARQ6v5w0po00001802",
			"size" : 6146,
			"executable" : false
		},
		{
			"hash" : "sqp0s8fvURMS5s7IVdnE0Ki12.oPfmk4lvqDKfBfmzk00001cb7",
			"size" : 7351,
			"executable" : false
		}
	]
}
 );
}
