/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef STORAGE_BACKEND_LOCAL_HH
#define STORAGE_BACKEND_LOCAL_HH

#include "backend.hh"
#include "net/aws.hh"
#include "net/s3.hh"

class LocalStorageBackend : public StorageBackend
{
public:
  LocalStorageBackend() {}

  void put( const std::vector<storage::PutRequest> &,
            const PutCallback & = []( const storage::PutRequest & ){} ) {}

  void get( const std::vector<storage::GetRequest> &,
            const GetCallback & = []( const storage::GetRequest & ){} ) {}
};

#endif /* STORAGE_BACKEND_LOCAL_HH */
