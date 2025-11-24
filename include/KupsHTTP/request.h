#ifndef KUPSHTTP_REQUEST_H
#define KUPSHTTP_REQUEST_H

#include <string>
#include <unordered_map>

namespace KupsHTTP
{

  class Request
  {
  public:
    Request() = default;
    ~Request() = default;

    // HTTP Method
    enum class Method
    {
      GET,
      POST,
      PUT,
      DELETE,
      OPTIONS,
      HEAD,
      PATCH,
      UNKNOWN
    };

    // Method to string conversion
    static std::string methodToString(Method method)
    {
      switch (method)
      {
      case Method::GET:
        return "GET";
      case Method::POST:
        return "POST";
      case Method::PUT:
        return "PUT";
      case Method::DELETE:
        return "DELETE";
      case Method::OPTIONS:
        return "OPTIONS";
      case Method::HEAD:
        return "HEAD";
      case Method::PATCH:
        return "PATCH";
      default:
        return "UNKNOWN";
      }
    }

    // Getters
    Method getMethod() const { return method_; }
    std::string getMethodString() const { return methodToString(method_); }
    const std::string &getPath() const { return path_; }
    const std::string &getVersion() const { return version_; }
    const std::string &getBody() const { return body_; }

    // Header operations
    void setHeader(const std::string &key, const std::string &value);
    std::string getHeader(const std::string &key) const;
    bool hasHeader(const std::string &key) const;

    // Cookie operations
    void setCookie(const std::string &key, const std::string &value);
    std::string getCookie(const std::string &key) const;
    bool hasCookie(const std::string &key) const;

    // Query parameters
    std::string getQueryParam(const std::string &key) const;
    bool hasQueryParam(const std::string &key) const;

    // Parse raw HTTP request
    static Request parse(const std::string &rawRequest);

  private:
    Method method_ = Method::UNKNOWN;
    std::string path_;
    std::string version_;
    std::unordered_map<std::string, std::string> headers_;
    std::unordered_map<std::string, std::string> cookies_;
    std::unordered_map<std::string, std::string> queryParams_;
    std::string body_;
  };

} // namespace KupsHTTP

#endif // KUPSHTTP_REQUEST_H