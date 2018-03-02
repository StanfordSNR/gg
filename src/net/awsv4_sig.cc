/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "awsv4_sig.hh"

#include <cstdio>
#include <openssl/sha.h>
#include <openssl/hmac.h>
#include <sstream>
#include <iostream>

using namespace std;

static const string ALGORITHM_ = "AWS4-HMAC-SHA256";

string
AWSv4Sig::sha256buf_to_string_(const unsigned char *buf) {
    char sbuf[2*SHA256_DIGEST_LENGTH + 1];
    for (unsigned i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        snprintf(&(sbuf[2*i]), 3, "%2.2x", buf[i]);
    }
    return string(sbuf, 2*SHA256_DIGEST_LENGTH);
}

string
AWSv4Sig::sha256_(const string &in) {
    unsigned char buf[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char *)in.c_str(), in.length(), buf);
    return sha256buf_to_string_(buf);
}

vector<uint8_t>
AWSv4Sig::derive_signing_key_(const string &secret,
                              const string &date,
                              const string &region,
                              const string &service) {
    // abundance of caution: don't read from and write to same buffer
    unsigned char buf0[SHA256_DIGEST_LENGTH], buf1[SHA256_DIGEST_LENGTH];
    vector<uint8_t> out(SHA256_DIGEST_LENGTH);

    // kDate = HMAC("AWS4" + secret, date)
    HMAC(EVP_sha256(), ("AWS4" + secret).c_str(), 4 + secret.length(),
         (const unsigned char *)date.c_str(), date.length(), buf0, nullptr);

    // kRegion = HMAC(kDate, region)
    HMAC(EVP_sha256(), buf0, SHA256_DIGEST_LENGTH,
         (const unsigned char *)region.c_str(), region.length(), buf1, nullptr);

    // kService = HMAC(kRegion, service)
    HMAC(EVP_sha256(), buf1, SHA256_DIGEST_LENGTH,
         (const unsigned char *)service.c_str(), service.length(), buf0, nullptr);

    // kSigning = HMAC(kService, "aws4_request")
    HMAC(EVP_sha256(), buf0, SHA256_DIGEST_LENGTH,
         (const unsigned char *)"aws4_request", 12, out.data(), nullptr);

    return out;
}

void
AWSv4Sig::sign_request(const std::string & first_line,
                       const std::string & secret,
                       const std::string & akid,
                       const std::string & region,
                       const std::string & service,
                       const std::string & request_date,
                       const std::string & payload __attribute((unused)),
                       std::map<std::string, std::string> & headers,
                       const std::string & payload_hash) {
    // begin building canonical request
    stringstream req;
    req << first_line << '\n' << '\n';

    // build up signed_headers list and canonical headers
    string signed_headers;
    {
        stringstream shead;
        bool first = true;

        for (const auto &hd: headers) {
            // XXX hd.first should be lowercased
            // XXX hd.second should be trimmed (no whitespace on sides, collapse middle ws)
            req << hd.first << ':' << hd.second << '\n';

            if (!first) {
                shead << ';';
            }
            first = false;

            shead << hd.first;
        }

        signed_headers = shead.str();
    }
    // add in signed headers and payload hash
    string hash_val;

    if ( payload_hash.length() == 0 ) {
      hash_val = sha256_( payload );
    }
    else {
      hash_val = payload_hash;
    }


    req << '\n'
        << signed_headers << '\n'
        << hash_val;

    // hash canonical request
    string canon_req_hash = sha256_(req.str());

    // build up credential scope
    req.str("");
    req << request_date.substr(0, 8) << '/' << region << '/' << service << "/aws4_request";
    string cred_scope = req.str();

    // build up string to sign
    req.str("");
    req << ALGORITHM_ << '\n'
        << request_date << '\n'
        << cred_scope << '\n'
        << canon_req_hash;
    string string_to_sign = req.str();

    // derive key and create signature
    unsigned char buf[SHA256_DIGEST_LENGTH];
    vector<uint8_t> skey = derive_signing_key_(secret,
                                               request_date.substr(0, 8),
                                               region,
                                               service);
    HMAC(EVP_sha256(), skey.data(), SHA256_DIGEST_LENGTH,
         (const unsigned char *)string_to_sign.c_str(), string_to_sign.length(),
         buf, nullptr);
    string signature = sha256buf_to_string_(buf);

    // build up authorization header
    req.str("");
    req << ALGORITHM_
        << " Credential=" << akid << '/' << cred_scope
        << ", SignedHeaders=" << signed_headers
        << ", Signature=" << signature;

    headers["authorization"] = req.str();
    headers["x-amz-content-sha256"] = hash_val;
}
