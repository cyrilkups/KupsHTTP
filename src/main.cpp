#include "http_server.hpp"
#include <iostream>

int main()
{
  try
  {
    HTTPServer server(8080);
    server.start();
  }
  catch (const std::exception &e)
  {
    std::cerr << "Server error: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}