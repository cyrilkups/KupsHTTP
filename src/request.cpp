#include <KupsHTTP/request.h>
#include <sstream>
#include <algorithm>

namespace KupsHTTP
{

  Request Request::parse(const std::string &rawRequest)
  {
    Request request;
    std::istringstream ss(rawRequest);
    std::string line;

    // Parse request line
    if (std::getline(ss, line))
    {
      std::istringstream lineStream(line);
      std::string method, path, version;
      lineStream >> method >> path >> version;

      // Parse method
      if (method == "GET")
        request.method_ = Method::GET;
      else if (method == "POST")
        request.method_ = Method::POST;
      else if (method == "PUT")
        request.method_ = Method::PUT;
      else if (method == "DELETE")
        request.method_ = Method::DELETE;
      else if (method == "OPTIONS")
        request.method_ = Method::OPTIONS;
      else if (method == "HEAD")
        request.method_ = Method::HEAD;
      else if (method == "PATCH")
        request.method_ = Method::PATCH;
      else
        request.method_ = Method::UNKNOWN;

      // Parse path and query parameters
      size_t queryPos = path.find('?');
      if (queryPos != std::string::npos)
      {
        request.path_ = path.substr(0, queryPos);
        std::string queryString = path.substr(queryPos + 1);

        // Parse query parameters
        std::istringstream queryStream(queryString);
        std::string param;
        while (std::getline(queryStream, param, '&'))
        {
          size_t equalPos = param.find('=');
          if (equalPos != std::string::npos)
          {
            std::string key = param.substr(0, equalPos);
            std::string value = param.substr(equalPos + 1);
            request.queryParams_[key] = value;
          }
        }
      }
      else
      {
        request.path_ = path;
      }

      request.version_ = version;
    }

    // Parse headers
    while (std::getline(ss, line) && line != "\r")
    {
      size_t colonPos = line.find(':');
      if (colonPos != std::string::npos)
      {
        std::string key = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 1);

        // Trim whitespace
        value.erase(0, value.find_first_not_of(" \t\r\n"));
        value.erase(value.find_last_not_of(" \t\r\n") + 1);

        request.headers_[key] = value;

        // Parse cookies if present
        if (key == "Cookie")
        {
          std::istringstream cookieStream(value);
          std::string cookie;
          while (std::getline(cookieStream, cookie, ';'))
          {
            size_t equalPos = cookie.find('=');
            if (equalPos != std::string::npos)
            {
              std::string cookieKey = cookie.substr(0, equalPos);
              std::string cookieValue = cookie.substr(equalPos + 1);

              // Trim whitespace
              cookieKey.erase(0, cookieKey.find_first_not_of(" \t"));
              cookieKey.erase(cookieKey.find_last_not_of(" \t") + 1);
              cookieValue.erase(0, cookieValue.find_first_not_of(" \t"));
              cookieValue.erase(cookieValue.find_last_not_of(" \t") + 1);

              request.cookies_[cookieKey] = cookieValue;
            }
          }
        }
      }
    }

    // Read body if present
    std::stringstream bodyStream;
    while (std::getline(ss, line))
    {
      bodyStream << line << "\n";
    }
    request.body_ = bodyStream.str();

    return request;
  }

  void Request::setHeader(const std::string &key, const std::string &value)
  {
    headers_[key] = value;
  }

  std::string Request::getHeader(const std::string &key) const
  {
    auto it = headers_.find(key);
    return it != headers_.end() ? it->second : "";
  }

  bool Request::hasHeader(const std::string &key) const
  {
    return headers_.find(key) != headers_.end();
  }

  void Request::setCookie(const std::string &key, const std::string &value)
  {
    cookies_[key] = value;
  }

  std::string Request::getCookie(const std::string &key) const
  {
    auto it = cookies_.find(key);
    return it != cookies_.end() ? it->second : "";
  }

  bool Request::hasCookie(const std::string &key) const
  {
    return cookies_.find(key) != cookies_.end();
  }

  std::string Request::getQueryParam(const std::string &key) const
  {
    auto it = queryParams_.find(key);
    return it != queryParams_.end() ? it->second : "";
  }

  bool Request::hasQueryParam(const std::string &key) const
  {
    return queryParams_.find(key) != queryParams_.end();
  }

} // namespace KupsHTTP