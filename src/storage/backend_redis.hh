/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef STORAGE_BACKEND_REDIS_HH
#define STORAGE_BACKEND_REDIS_HH

#include "storage/backend.hh"
#include "net/redis.hh"

class RedisStorageBackend : public StorageBackend
{
private:
  Redis client_;

public:
  RedisStorageBackend( RedisClientConfig & config )
    : client_( config )
  {}

  void put( const std::vector<storage::PutRequest> & requests,
            const PutCallback & success_callback = []( const storage::PutRequest & ){} ) override;

  void get( const std::vector<storage::GetRequest> & requests,
            const GetCallback & success_callback = []( const storage::GetRequest & ){} ) override;

};

#endif /* STORAGE_BACKEND_REDIS_HH */
