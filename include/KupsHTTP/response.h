#ifndef KUPSHTTP_RESPONSE_H
#define KUPSHTTP_RESPONSE_H

#include <string>
#include <unordered_map>

namespace KupsHTTP
{

  class Response
  {
  public:
    Response(int statusCode = 200, const std::string &statusText = "OK");
    ~Response() = default;

    // Status
    void setStatus(int code, const std::string &text);
    int getStatusCode() const { return statusCode_; }
    const std::string &getStatusText() const { return statusText_; }

    // Headers
    void setHeader(const std::string &key, const std::string &value);
    std::string getHeader(const std::string &key) const;
    bool hasHeader(const std::string &key) const;

    // Content
    void setContent(const std::string &content, const std::string &type = "text/plain");
    const std::string &getContent() const { return content_; }

    // Cookies
    void setCookie(const std::string &name, const std::string &value,
                   const std::string &path = "/", int maxAge = -1,
                   bool secure = false, bool httpOnly = false);

    // Utility methods
    std::string toString() const;
    static Response makeError(int code, const std::string &message);
    static Response makeRedirect(const std::string &location, int code = 302);
    static Response makeFile(const std::string &filePath);

  private:
    int statusCode_;
    std::string statusText_;
    std::unordered_map<std::string, std::string> headers_;
    std::string content_;
  };

} // namespace KupsHTTP

#endif // KUPSHTTP_RESPONSE_H