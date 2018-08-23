AM_CPPFLAGS = -I$(srcdir)/. -I$(srcdir)/.. $(CXX14_FLAGS) $(SSL_CFLAGS) $(HIREDIS_CFLAGS)
AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(EXTRA_CXXFLAGS)

noinst_LIBRARIES = libggnet.a

libggnet_a_SOURCES = address.cc address.hh body_parser.hh \
                     chunked_parser.cc chunked_parser.hh \
                     http_header.cc http_header.hh \
                     http_message.cc http_message.hh \
                     http_message_sequence.hh http_request.cc http_request.hh \
                     http_request_parser.hh http_response.cc http_response.hh \
                     http_response_parser.cc http_response_parser.hh \
                     mime_type.cc mime_type.hh \
                     secure_socket.cc secure_socket.hh socket.cc socket.hh \
                     nb_secure_socket.cc nb_secure_socket.hh \
                     strict_conversions.hh strict_conversions.cc \
                     requests.hh \
                     aws.hh aws.cc \
                     awsv4_sig.hh awsv4_sig.cc \
                     s3.hh s3.cc \
                     lambda.hh lambda.cc \
                     redis.hh redis.cc \
                     gcloud.hh gcloud.cc
