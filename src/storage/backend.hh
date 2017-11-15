/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef STORAGE_BACKEND_HH
#define STORAGE_BACKEND_HH

#include <vector>

class StorageBackend
{
public:
  virtual void put( const PutRequest & request );
  virtual void put( const std::vector<PutRequest> & requests );

  virtual void get( const GetRequest & request );
  virtual void get( const std::vector<GetRequest> & requests );
};

#endif /* STORAGE_BACKEND_HH */
