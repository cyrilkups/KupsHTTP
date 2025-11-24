#include <KupsHTTP/response.h>
#include <sstream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;
namespace KupsHTTP
{

  Response::Response(int statusCode, const std::string &statusText)
      : statusCode_(statusCode), statusText_(statusText)
  {
    headers_["Content-Type"] = "text/plain";
  }

  void Response::setStatus(int code, const std::string &text)
  {
    statusCode_ = code;
    statusText_ = text;
  }

  void Response::setHeader(const std::string &key, const std::string &value)
  {
    headers_[key] = value;
  }

  std::string Response::getHeader(const std::string &key) const
  {
    auto it = headers_.find(key);
    return it != headers_.end() ? it->second : "";
  }

  bool Response::hasHeader(const std::string &key) const
  {
    return headers_.find(key) != headers_.end();
  }

  void Response::setContent(const std::string &content, const std::string &type)
  {
    content_ = content;
    headers_["Content-Type"] = type;
    headers_["Content-Length"] = std::to_string(content.length());
  }

  void Response::setCookie(const std::string &name, const std::string &value,
                           const std::string &path, int maxAge,
                           bool secure, bool httpOnly)
  {
    std::stringstream ss;
    ss << name << "=" << value;

    if (!path.empty())
      ss << "; Path=" << path;
    if (maxAge >= 0)
      ss << "; Max-Age=" << maxAge;
    if (secure)
      ss << "; Secure";
    if (httpOnly)
      ss << "; HttpOnly";

    headers_["Set-Cookie"] = ss.str();
  }

  std::string Response::toString() const
  {
    std::stringstream ss;
    ss << "HTTP/1.1 " << statusCode_ << " " << statusText_ << "\r\n";

    for (const auto &[key, value] : headers_)
    {
      ss << key << ": " << value << "\r\n";
    }

    ss << "\r\n"
       << content_;
    return ss.str();
  }

  Response Response::makeError(int code, const std::string &message)
  {
    Response response(code, message);
    response.setContent(message);
    return response;
  }

  Response Response::makeRedirect(const std::string &location, int code)
  {
    Response response(code, "Found");
    response.setHeader("Location", location);
    return response;
  }

  Response Response::makeFile(const std::string &filePath)
  {
    if (!fs::exists(filePath) || !fs::is_regular_file(filePath))
    {
      return makeError(404, "File not found");
    }

    std::ifstream file(filePath, std::ios::binary);
    if (!file)
    {
      return makeError(500, "Failed to read file");
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    Response response(200, "OK");

    // Determine content type based on file extension
    std::string ext = filePath.substr(filePath.find_last_of(".") + 1);
    std::string contentType = "application/octet-stream";

    static const std::unordered_map<std::string, std::string> mimeTypes = {
        {"html", "text/html"},
        {"htm", "text/html"},
        {"css", "text/css"},
        {"js", "application/javascript"},
        {"json", "application/json"},
        {"png", "image/png"},
        {"jpg", "image/jpeg"},
        {"jpeg", "image/jpeg"},
        {"gif", "image/gif"},
        {"svg", "image/svg+xml"},
        {"ico", "image/x-icon"},
        {"txt", "text/plain"},
        {"pdf", "application/pdf"},
        {"zip", "application/zip"}};

    auto it = mimeTypes.find(ext);
    if (it != mimeTypes.end())
    {
      contentType = it->second;
    }

    response.setContent(buffer.str(), contentType);
    return response;
  }

} // namespace KupsHTTP