#include "http_server.hpp"
#include "request_handler.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

HTTPServer::HTTPServer(int port) : port_(port), running_(false)
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

  sockaddr_in serverAddr;
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = INADDR_ANY;
  serverAddr.sin_port = htons(port_);

  if (bind(serverSocket_, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
  {
    throw std::runtime_error("Failed to bind socket");
  }
}

HTTPServer::~HTTPServer()
{
  stop();
  close(serverSocket_);
}

void HTTPServer::start()
{
  running_ = true;
  if (listen(serverSocket_, 10) < 0)
  {
    throw std::runtime_error("Failed to listen on socket");
  }

  std::cout << "Server listening on port " << port_ << std::endl;
  acceptConnections();
}

void HTTPServer::stop()
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

void HTTPServer::acceptConnections()
{
  while (running_)
  {
    sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);

    int clientSocket = accept(serverSocket_, (struct sockaddr *)&clientAddr, &clientLen);
    if (clientSocket < 0)
    {
      std::cerr << "Failed to accept connection" << std::endl;
      continue;
    }

    clientThreads_.emplace_back(&HTTPServer::handleClient, this, clientSocket);
  }
}

void HTTPServer::handleClient(int clientSocket)
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

  // Parse and handle the request
  HTTPRequest request;
  // TODO: Parse request data into HTTPRequest object

  RequestHandler handler;
  HTTPResponse response = handler.handleRequest(request);

  // Send response
  std::string responseStr = "HTTP/1.1 " + std::to_string(response.statusCode) + " " + response.statusText + "\r\n";
  for (const auto &header : response.headers)
  {
    responseStr += header.first + ": " + header.second + "\r\n";
  }
  responseStr += "\r\n" + response.body;

  send(clientSocket, responseStr.c_str(), responseStr.length(), 0);
  close(clientSocket);
}