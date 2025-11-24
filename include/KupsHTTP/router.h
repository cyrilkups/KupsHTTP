#ifndef KUPSHTTP_ROUTER_H
#define KUPSHTTP_ROUTER_H

#include "request.h"
#include "response.h"

#include <string>
#include <functional>
#include <unordered_map>
#include <vector>
#include <regex>

namespace KupsHTTP
{

  class Router
  {
  public:
    Router() = default;
    ~Router() = default;

    // Route registration
    void addRoute(const std::string &method, const std::string &path,
                  std::function<Response(const Request &)> handler);

    // Route matching and handling
    Response route(const Request &request);

    // Middleware support
    using Middleware = std::function<bool(Request &, Response &)>;
    void use(Middleware middleware);

  private:
    struct Route
    {
      std::string method;
      std::regex pathPattern;
      std::function<Response(const Request &)> handler;
    };

    bool matchRoute(const std::string &path, const std::regex &pattern) const;
    std::vector<Route> routes_;
    std::vector<Middleware> middleware_;
  };

} // namespace KupsHTTP

#endif // KUPSHTTP_ROUTER_H