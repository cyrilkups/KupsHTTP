#include <KupsHTTP/server.h>
#include <iostream>
#include <fstream>
#include <ctime>

int main()
{
  try
  {
    KupsHTTP::Server server("localhost", 8080);

    // Add routes
    server.get("/", [](const KupsHTTP::Request &req)
               {
            KupsHTTP::Response res(200, "OK");
            res.setContent("<h1>Welcome to KupsHTTP!</h1>", "text/html");
            return res; });

    server.get("/time", [](const KupsHTTP::Request &req)
               {
            time_t now = time(nullptr);
            std::string timeStr = ctime(&now);
            
            KupsHTTP::Response res(200, "OK");
            res.setContent(timeStr, "text/plain");
            return res; });

    server.post("/echo", [](const KupsHTTP::Request &req)
                {
            KupsHTTP::Response res(200, "OK");
            res.setContent(req.getBody(), req.getHeader("Content-Type"));
            return res; });

    // Add middleware for logging
    server.getRouter().use([](KupsHTTP::Request &req, KupsHTTP::Response &res)
                           {
            std::cout << "[" << time(nullptr) << "] " 
                     << req.getMethodString() << " " 
                     << req.getPath() << std::endl;
            return true; });

    std::cout << "Starting server on http://localhost:8080" << std::endl;
    server.start();
  }
  catch (const std::exception &e)
  {
    std::cerr << "Server error: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}