/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef STORAGE_BACKEND_HH
#define STORAGE_BACKEND_HH

#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <unordered_map>

#include "net/requests.hh"
#include "util/path.hh"
#include "util/optional.hh"

typedef std::function<void( const storage::PutRequest & )> PutCallback;
typedef std::function<void( const storage::GetRequest & )> GetCallback;

struct StorageEndpoint
{
  std::string protocol {};
  std::string username {};
  std::string password {};
  std::string host {};
  Optional<uint16_t> port { 0 };
  std::string path {};

  std::unordered_map<std::string, std::string> options {};
};

class StorageBackend
{
public:
  virtual void put( const std::vector<storage::PutRequest> & requests,
                    const PutCallback & success_callback = []( const storage::PutRequest & ){} ) = 0;

  virtual void get( const std::vector<storage::GetRequest> & requests,
                    const GetCallback & success_callback = []( const storage::GetRequest & ){} ) = 0;

  static std::unique_ptr<StorageBackend> create_backend( const std::string & uri );

  virtual ~StorageBackend() {}
};

#endif /* STORAGE_BACKEND_HH */
