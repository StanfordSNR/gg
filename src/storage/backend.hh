/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef STORAGE_BACKEND_HH
#define STORAGE_BACKEND_HH

#include <vector>
#include <string>
#include <functional>

#include "path.hh"
#include "optional.hh"

struct PutRequest
{
  roost::path filename;
  std::string object_key;
  Optional<std::string> content_hash;
};

struct GetRequest
{
  std::string object_key;
  roost::path filename;
};

class StorageBackend
{
public:
  typedef std::function<void( const PutRequest & )> PutCallback;
  typedef std::function<void( const GetRequest & )> GetCallback;

  virtual void put( const PutRequest & request ) = 0;
  virtual void put( const std::vector<PutRequest> & requests,
                    const PutCallback & success_callback ) = 0;

  virtual void get( const GetRequest & request ) = 0;
  virtual void get( const std::vector<GetRequest> & requests,
                    const GetCallback & success_callback ) = 0;

  virtual ~StorageBackend() {}
};

#endif /* STORAGE_BACKEND_HH */
