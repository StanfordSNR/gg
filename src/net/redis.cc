/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#include "net/redis.hh"

#include <thread>
#include <memory>
#include <stdexcept>
#include <cstring>
#include <fcntl.h>
#include <sys/time.h>
#include <hiredis/hiredis.h>

#include "util/exception.hh"
#include "util/file_descriptor.hh"
#include "util/optional.hh"

using namespace std;

void Redis::upload_files( const vector<storage::PutRequest> & upload_requests,
                          const function<void( const storage::PutRequest & )> & success_callback )
{
  const size_t thread_count = config_.max_threads;
  const size_t batch_size = config_.max_batch_size;
  struct timeval redis_timeout = { 1, 500000 }; /* 1.5 seconds */

  vector<thread> threads;
  for ( size_t thread_index = 0; thread_index < thread_count; thread_index++ ) {
    if ( thread_index < upload_requests.size() ) {
      threads.emplace_back(
        [&] ( const size_t index )
        {
          shared_ptr<redisContext> redis_context {
            redisConnectWithTimeout( config_.ip.c_str(), config_.port, redis_timeout ),
            redisFree
          };

          if ( redis_context == nullptr or redis_context->err ) {
            throw runtime_error( "error connecting to redis server" );
          }

          if ( config_.password.length() ) {
            shared_ptr<redisReply> auth_reply {
              (redisReply *)redisCommand( redis_context.get(), "AUTH %s", config_.password.c_str() ),
              freeReplyObject
            };

            if ( auth_reply == nullptr or
                 auth_reply->type == REDIS_REPLY_ERROR ) {
              throw runtime_error( "could not authenticate to redis server" );
            }
          }

          for ( size_t first_file_idx = index;
                first_file_idx < upload_requests.size();
                first_file_idx += thread_count * batch_size ) {

            size_t expected_responses = 0;

            for ( size_t file_id = first_file_idx;
                  file_id < min( upload_requests.size(), first_file_idx + thread_count * batch_size );
                  file_id += thread_count ) {
              const string & filename = upload_requests.at( file_id ).filename.string();
              const string & object_key = upload_requests.at( file_id ).object_key;

              string contents;
              FileDescriptor file { CheckSystemCall( "open " + filename, open( filename.c_str(), O_RDONLY ) ) };
              while ( not file.eof() ) { contents.append( file.read() ); }
              file.close();

              redisAppendCommand( redis_context.get(),
                                  "SET %s %b", object_key.c_str(),
                                  contents.data(), contents.length() );
              expected_responses++;
            }

            size_t response_count = 0;

            while ( response_count != expected_responses ) {
              /* drain responses */
              redisReply * reply_ptr;

              if ( redisGetReply( redis_context.get(), (void **)&reply_ptr ) != REDIS_OK ) {
                throw runtime_error( "failed to get response from redis" );
              }

              shared_ptr<redisReply> reply { reply_ptr, freeReplyObject };

              const size_t response_index = first_file_idx + response_count * thread_count;
              success_callback( upload_requests[ response_index ] );

              response_count++;
            }
          }
        }, thread_index
      );
    }
  }

  for ( auto & thread : threads ) {
    thread.join();
  }
}

void Redis::download_files( const vector<storage::GetRequest> & download_requests,
                            const function<void( const storage::GetRequest & )> & success_callback )
{
  const size_t thread_count = config_.max_threads;
  const size_t batch_size = config_.max_batch_size;
  struct timeval redis_timeout = { 1, 500000 }; /* 1.5 seconds */

  vector<thread> threads;
  for ( size_t thread_index = 0; thread_index < thread_count; thread_index++ ) {
    if ( thread_index < download_requests.size() ) {
      threads.emplace_back(
        [&] ( const size_t index )
        {
          shared_ptr<redisContext> redis_context {
            redisConnectWithTimeout( config_.ip.c_str(), config_.port, redis_timeout ),
            redisFree
          };

          if ( redis_context == nullptr or redis_context->err ) {
            throw runtime_error( "error connecting to redis server" );
          }

          if ( config_.password.length() ) {
            shared_ptr<redisReply> auth_reply {
              (redisReply *)redisCommand( redis_context.get(), "AUTH %s", config_.password.c_str() ),
              freeReplyObject
            };

            if ( auth_reply == nullptr or
                 auth_reply->type == REDIS_REPLY_ERROR ) {
              throw runtime_error( "could not authenticate to redis server" );
            }
          }

          for ( size_t first_file_idx = index;
                first_file_idx < download_requests.size();
                first_file_idx += thread_count * batch_size ) {

            size_t expected_responses = 0;

            for ( size_t file_id = first_file_idx;
                  file_id < min( download_requests.size(), first_file_idx + thread_count * batch_size );
                  file_id += thread_count ) {
              const string & object_key = download_requests.at( file_id ).object_key;
              redisAppendCommand( redis_context.get(), "GET %s", object_key.c_str() );
              expected_responses++;
            }

            size_t response_count = 0;

            while ( response_count != expected_responses ) {
              /* drain responses */
              redisReply * reply_ptr;

              if ( redisGetReply( redis_context.get(), (void **)&reply_ptr ) != REDIS_OK ) {
                throw runtime_error( "failed to get response from redis" );
              }

              shared_ptr<redisReply> reply { reply_ptr, freeReplyObject };

              if ( reply->type != REDIS_REPLY_STRING or
                   reply->str == nullptr ) {
                throw runtime_error( "unexpected response from redis" );
              }

              string str_data { reply->str, static_cast<uint32_t>( reply->len ) };

              const size_t response_index = first_file_idx + response_count * thread_count;
              const string & filename = download_requests.at( response_index ).filename.string();

              roost::atomic_create( str_data, filename,
                                    download_requests[ response_index ].mode.initialized(),
                                    download_requests[ response_index ].mode.get_or( 0 ) );

              success_callback( download_requests[ response_index ] );

              response_count++;
            }
          }
        }, thread_index
      );
    }
  }

  for ( auto & thread : threads ) {
    thread.join();
  }
}
