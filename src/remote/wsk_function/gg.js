#!/usr/bin/env node

var path = require( 'path' );
var fs = require( 'fs' );

var gg_dir;
var blobs_dir;
var reductions_dir;

exports.init = function( gg_directory ) {
  gg_dir = gg_directory;

  if ( !gg_dir ) {
    throw new Error( "GG_DIR environment variable not set" );
  }

  if ( !path.isAbsolute( gg_dir ) ) {
    throw new Error( "GG_DIR must be an absolute path" );
  }

  blobs_dir = path.join( gg_dir, 'blobs' );
  reductions_dir = path.join( gg_dir, 'reductions' );

  [ gg_dir, blobs_dir, reductions_dir ].forEach( ( dir ) => {
    try {
      fs.mkdirSync( dir );
    }
    catch (  err ) {
      if ( !( err instanceof Error && err.code == 'EEXIST' ) ) { throw err; }
    }
  } );

  return this;
}

exports.blob_path = function( hash ) {
  if ( !blobs_dir ) {
    init();
  }

  return path.join( blobs_dir, hash );
}

exports.reduction_path = function( hash ) {
  if ( !reductions_dir ) {
    init();
  }

  return path.join( reductions_dir, hash );
}

exports.object_url = function( bucket, key ) {
  return 'https://' + bucket + '.s3.amazonaws.com/' + key;
}

exports.make_executable = function( filepath ) {
  var stat_data = fs.statSync( filepath );
  fs.chmodSync( filepath, 0755 );
}

exports.check_cache = function( hash ) {
  try {
    var data = fs.readFileSync( this.reduction_path( hash ), { 'encoding': 'utf-8' } );
    return data.split( " " )[ 0 ];
  }
  catch ( err ) {
    console.log( err );
    return null;
  }
}

exports.get_blobs_dir = function() {
  if ( !blobs_dir ) {
    init();
  }

  return blobs_dir;
}
