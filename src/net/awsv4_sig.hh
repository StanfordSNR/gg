/* -*-mode:c++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */

#ifndef AWSV4_SIG_HH
#define AWSV4_SIG_HH

#include <map>
#include <string>
#include <vector>

class AWSv4Sig {
  private:
    static std::vector<uint8_t> derive_signing_key_(const std::string &secret,
                                                    const std::string &date,
                                                    const std::string &region,
                                                    const std::string &service);
    static std::string sha256buf_to_string_(const unsigned char *buf);
    static std::string sha256_(const std::string &in);

  public:
    static void sign_request(const std::string &first_line,
                             const std::string &secret,
                             const std::string &akid,
                             const std::string &region,
                             const std::string &service,
                             const std::string &request_date,
                             const std::string &payload,
                             std::map<std::string, std::string> &headers,
                             const std::string & payload_hash = {});
};

#endif /* AWSV4_SIG_HH */
