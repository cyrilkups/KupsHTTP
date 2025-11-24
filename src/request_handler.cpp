#include "request_handler.hpp"

#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

HTTPResponse RequestHandler::handleRequest(const HTTPRequest &request)
{
  if (request.method == "GET")
  {
    return handleGET(request);
  }
  else if (request.method == "POST")
  {
    return handlePOST(request);
  }
  else if (request.method == "DELETE")
  {
    return handleDELETE(request);
  }

  return {405, "Method Not Allowed", {{"Content-Type", "text/plain"}}, "Method not allowed"};
}

HTTPResponse RequestHandler::handleGET(const HTTPRequest &request)
{
  if (isStaticFile(request.path))
  {
    std::string content = readFile(request.path);
    if (content.empty())
    {
      return {404, "Not Found", {{"Content-Type", "text/plain"}}, "File not found"};
    }

    return {200, "OK", {{"Content-Type", getContentType(request.path)}, {"Content-Length", std::to_string(content.length())}}, content};
  }

  return {404, "Not Found", {{"Content-Type", "text/plain"}}, "Resource not found"};
}

HTTPResponse RequestHandler::handlePOST(const HTTPRequest &request)
{
  auto contentType = request.headers.find("Content-Type");
  if (contentType != request.headers.end() &&
      contentType->second.find("multipart/form-data") != std::string::npos)
  {
    handleFileUpload(request);
    return {200, "OK", {{"Content-Type", "text/plain"}}, "File uploaded successfully"};
  }

  return {400, "Bad Request", {{"Content-Type", "text/plain"}}, "Invalid request"};
}

HTTPResponse RequestHandler::handleDELETE(const HTTPRequest &request)
{
  if (isStaticFile(request.path))
  {
    if (fs::remove(request.path))
    {
      return {200, "OK", {{"Content-Type", "text/plain"}}, "File deleted successfully"};
    }
    return {500, "Internal Server Error", {{"Content-Type", "text/plain"}}, "Failed to delete file"};
  }

  return {404, "Not Found", {{"Content-Type", "text/plain"}}, "Resource not found"};
}

std::string RequestHandler::getContentType(const std::string &path)
{
  std::string ext = path.substr(path.find_last_of(".") + 1);
  std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

  static const std::unordered_map<std::string, std::string> contentTypes = {
      {"html", "text/html"},
      {"css", "text/css"},
      {"js", "application/javascript"},
      {"json", "application/json"},
      {"png", "image/png"},
      {"jpg", "image/jpeg"},
      {"jpeg", "image/jpeg"},
      {"gif", "image/gif"}};

  auto it = contentTypes.find(ext);
  return it != contentTypes.end() ? it->second : "application/octet-stream";
}

bool RequestHandler::isStaticFile(const std::string &path)
{
  return fs::exists(path) && fs::is_regular_file(path);
}

std::string RequestHandler::readFile(const std::string &path)
{
  std::ifstream file(path, std::ios::binary);
  if (!file)
    return "";

  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

void RequestHandler::handleFileUpload(const HTTPRequest &request)
{
  // TODO: Implement multipart form-data parsing and file upload handling
  // This would involve:
  // 1. Parsing the boundary from Content-Type header
  // 2. Splitting the body into parts using the boundary
  // 3. Parsing each part's headers and content
  // 4. Saving the file content to disk
}