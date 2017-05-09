/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef SERIALIZATION_HH
#define SERIALIZATION_HH

#include <string>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "file_descriptor.hh"

class ProtobufSerializer
{
protected:
  FileDescriptor fout_;
  google::protobuf::io::FileOutputStream raw_output_ { fout_.fd_num() };
  google::protobuf::io::CodedOutputStream coded_output_ { &raw_output_ };

public:
  ProtobufSerializer( const std::string & filename );
  ProtobufSerializer( FileDescriptor && fd );
  void write_string( const std::string & str );

  template<class EntryProtobufType>
  void write_protobuf( const EntryProtobufType & entry );
};

template<class EntryProtobufType>
void ProtobufSerializer::write_protobuf( const EntryProtobufType & protobuf )
{
  coded_output_.WriteLittleEndian32( protobuf.ByteSize() );
  if ( not protobuf.SerializeToCodedStream( &coded_output_ ) ) {
    throw std::runtime_error( "write_protobuf: write error" );
  }
}

class ProtobufDeserializer
{
protected:
  FileDescriptor fin_;
  google::protobuf::io::FileInputStream raw_input_ { fin_.fd_num() };

public:
  ProtobufDeserializer( const std::string & filename );
  ProtobufDeserializer( FileDescriptor && fd );
  std::string read_string( const size_t size );

  template<class EntryProtobufType>
  bool read_protobuf( EntryProtobufType & protobuf );
};

template<class EntryProtobufType>
bool ProtobufDeserializer::read_protobuf( EntryProtobufType & message )
{
  google::protobuf::uint32 size;
  google::protobuf::io::CodedInputStream coded_input { &raw_input_ };

  bool has_next = coded_input.ReadLittleEndian32( &size );

  if ( not has_next ) {
    return false;
  }

  google::protobuf::io::CodedInputStream::Limit message_limit =
    coded_input.PushLimit( size );

  if ( message.ParseFromCodedStream( &coded_input ) ) {
    coded_input.PopLimit( message_limit );
    return true;
  }

  return false;
}


#endif /* SERIALIZATION_HH */
