#include <KupsHTTP/router.h>
#include <iostream>

namespace KupsHTTP
{

  void Router::addRoute(const std::string &method, const std::string &path,
                        std::function<Response(const Request &)> handler)
  {
    // Convert path to regex pattern
    std::string pattern = path;

    // Replace :param with ([^/]+) to capture parameters
    size_t pos = 0;
    while ((pos = pattern.find(":", pos)) != std::string::npos)
    {
      size_t end = pattern.find("/", pos);
      if (end == std::string::npos)
        end = pattern.length();
      pattern.replace(pos, end - pos, "([^/]+)");
      pos += 8; // length of ([^/]+)
    }

    routes_.push_back({method, std::regex(pattern), handler});
  }

  Response Router::route(const Request &request)
  {
    // Apply middleware
    Request mutableRequest = request;
    Response response(200, "OK");

    for (const auto &middleware : middleware_)
    {
      if (!middleware(mutableRequest, response))
      {
        return response; // Middleware chain was interrupted
      }
    }

    // Find matching route
    for (const auto &route : routes_)
    {
      if (request.getMethod() == Request::Method::OPTIONS)
      {
        // Handle OPTIONS request
        Response response(204, "No Content");
        response.setHeader("Allow", "GET, POST, PUT, DELETE, OPTIONS");
        return response;
      }

      std::string methodStr;
      switch (request.getMethod())
      {
      case Request::Method::GET:
        methodStr = "GET";
        break;
      case Request::Method::POST:
        methodStr = "POST";
        break;
      case Request::Method::PUT:
        methodStr = "PUT";
        break;
      case Request::Method::DELETE:
        methodStr = "DELETE";
        break;
      default:
        continue;
      }

      if (route.method == methodStr && matchRoute(request.getPath(), route.pathPattern))
      {
        try
        {
          return route.handler(request);
        }
        catch (const std::exception &e)
        {
          std::cerr << "Error in route handler: " << e.what() << std::endl;
          return Response::makeError(500, "Internal Server Error");
        }
      }
    }

    // No matching route found
    return Response::makeError(404, "Not Found");
  }

  void Router::use(Middleware middleware)
  {
    middleware_.push_back(middleware);
  }

  bool Router::matchRoute(const std::string &path, const std::regex &pattern) const
  {
    return std::regex_match(path, pattern);
  }

} // namespace KupsHTTP