#include <KupsHTTP/server.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <filesystem>

namespace KupsHTTP
{

  Server::Server(const std::string &host, int port)
      : host_(host), port_(port), running_(false)
  {
    loadConfig();
  }

  Server::~Server()
  {
    stop();
    if (serverSocket_ >= 0)
    {
      close(serverSocket_);
    }
  }

  void Server::start()
  {
    serverSocket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket_ < 0)
    {
      throw std::runtime_error("Failed to create socket");
    }

    // Allow socket reuse
    int opt = 1;
    if (setsockopt(serverSocket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
      throw std::runtime_error("Failed to set socket options");
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port_);

    if (bind(serverSocket_, reinterpret_cast<struct sockaddr *>(&serverAddr), sizeof(serverAddr)) < 0)
    {
      throw std::runtime_error("Failed to bind socket");
    }

    if (listen(serverSocket_, 10) < 0)
    {
      throw std::runtime_error("Failed to listen on socket");
    }

    running_ = true;
    std::cout << "Server listening on " << host_ << ":" << port_ << std::endl;

    acceptConnections();
  }

  void Server::stop()
  {
    running_ = false;

    for (auto &thread : clientThreads_)
    {
      if (thread.joinable())
      {
        thread.join();
      }
    }
    clientThreads_.clear();
  }

  void Server::acceptConnections()
  {
    while (running_)
    {
      sockaddr_in clientAddr{};
      socklen_t clientLen = sizeof(clientAddr);

      int clientSocket = accept(serverSocket_, reinterpret_cast<struct sockaddr *>(&clientAddr), &clientLen);
      if (clientSocket < 0)
      {
        std::cerr << "Failed to accept connection" << std::endl;
        continue;
      }

      clientThreads_.emplace_back(&Server::handleClient, this, clientSocket);
    }
  }

  void Server::handleClient(int clientSocket)
  {
    const int bufferSize = 4096;
    char buffer[bufferSize];
    std::string requestData;

    while (true)
    {
      ssize_t bytesRead = recv(clientSocket, buffer, bufferSize - 1, 0);
      if (bytesRead <= 0)
        break;

      buffer[bytesRead] = '\0';
      requestData += buffer;

      if (requestData.find("\r\n\r\n") != std::string::npos)
        break;
    }

    try
    {
      Request request = Request::parse(requestData);
      Response response = router_.route(request);

      std::string responseStr = response.toString();
      send(clientSocket, responseStr.c_str(), responseStr.length(), 0);
    }
    catch (const std::exception &e)
    {
      std::cerr << "Error handling request: " << e.what() << std::endl;
      Response errorResponse = Response::makeError(500, "Internal Server Error");
      std::string errorStr = errorResponse.toString();
      send(clientSocket, errorStr.c_str(), errorStr.length(), 0);
    }

    close(clientSocket);
  }

  void Server::loadConfig()
  {
    std::ifstream configFile("config/server.conf");
    if (configFile)
    {
      std::string line;
      while (std::getline(configFile, line))
      {
        if (line.empty() || line[0] == '#')
          continue;

        size_t pos = line.find('=');
        if (pos != std::string::npos)
        {
          std::string key = line.substr(0, pos);
          std::string value = line.substr(pos + 1);

          // TODO: Parse configuration values
        }
      }
    }
  }

  void Server::get(const std::string &path, std::function<Response(const Request &)> handler)
  {
    router_.addRoute("GET", path, handler);
  }

  void Server::post(const std::string &path, std::function<Response(const Request &)> handler)
  {
    router_.addRoute("POST", path, handler);
  }

  void Server::put(const std::string &path, std::function<Response(const Request &)> handler)
  {
    router_.addRoute("PUT", path, handler);
  }

  void Server::del(const std::string &path, std::function<Response(const Request &)> handler)
  {
    router_.addRoute("DELETE", path, handler);
  }

} // namespace KupsHTTP