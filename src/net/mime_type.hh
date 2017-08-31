#ifndef MIME_TYPE_HH
#define MIME_TYPE_HH

#include <vector>
#include <string>

class MIMEType
{
private:
  std::string type_;
  std::vector< std::pair< std::string, std::string > > parameters_;

public:
  MIMEType( const std::string & content_type );

  const std::string & type() const { return type_; }
};

#endif /* MIME_TYPE_HH */
