#ifndef KUPSHTTP_SERVER_H
#define KUPSHTTP_SERVER_H

#include "request.h"
#include "response.h"
#include "router.h"

#include <string>
#include <thread>
#include <vector>
#include <functional>
#include <atomic>

namespace KupsHTTP
{

  class Server
  {
  public:
    Server(const std::string &host = "localhost", int port = 8080);
    ~Server();

    // Server configuration
    void setHost(const std::string &host) { host_ = host; }
    void setPort(int port) { port_ = port; }

    // Start/Stop server
    void start();
    void stop();

    // Router methods
    void get(const std::string &path, std::function<Response(const Request &)> handler);
    void post(const std::string &path, std::function<Response(const Request &)> handler);
    void put(const std::string &path, std::function<Response(const Request &)> handler);
    void del(const std::string &path, std::function<Response(const Request &)> handler);

    // Get router instance
    Router &getRouter() { return router_; }

  private:
    void acceptConnections();
    void handleClient(int clientSocket);
    void loadConfig();

    std::string host_;
    int port_;
    int serverSocket_;
    std::atomic<bool> running_;
    std::vector<std::thread> clientThreads_;
    Router router_;
  };

} // namespace KupsHTTP

#endif // KUPSHTTP_SERVER_H