# KupsHTTP

KupsHTTP is a small educational C++ HTTP server framework written with portability and clarity in mind. It provides a minimal, header-friendly API to create a basic HTTP server that supports common features like GET/POST/DELETE handlers, middleware, static files, file uploads, and cookie handling. The project targets modern C++ (C++17) and uses CMake for builds.

## Features

- Simple `Server` class with routing helpers (`get`, `post`, `put`, `del`).
- `Router` supporting route handlers and middleware.
- Support for serving static content and handling file uploads (basic implementation).
- Request and Response classes encapsulating headers, body, cookies, and method parsing.
- Thread-per-connection or thread-pool support (configurable in code).
- Lightweight and intended as a learning / prototyping server, not production-grade.

## Requirements

- A C++17-capable compiler (AppleClang, GCC 8+, or Clang 6+)
- CMake (3.10+ recommended)
- POSIX sockets (Linux, macOS; Windows not directly tested)

## Project layout

- `include/KupsHTTP/` — public headers (server, router, request, response, etc.)
- `src/` — implementation files
- `examples/` — example binary that demonstrates usage
- `public/` — example static files served by the server (if present)
- `CMakeLists.txt` — build configuration

## Build

From the project root (`KupsHTTP`):

```bash
mkdir -p build
cd build
cmake ..
make -j
```

This will produce a static library `libKupsHTTP.a` and an example executable named `KupsHTTP_example` (or similar depending on the CMake configuration) inside the `build` folder.

## Run the example

Run the example server from the `build` directory:

```bash
cd build
./KupsHTTP_example
```

The example binds to `localhost:8080` by default (this can be changed in `examples/main.cpp` or passed via API). Visit `http://localhost:8080` in a browser or use `curl` to exercise endpoints:

- GET `/` — welcome page
- GET `/time` — current server time
- POST `/echo` — echo request body

Example curl requests:

```bash
curl http://localhost:8080/
curl http://localhost:8080/time
curl -X POST -d "Hello world" http://localhost:8080/echo
```

## Quick API guide

Create a server:

```cpp
#include <KupsHTTP/server.h>

KupsHTTP::Server server("0.0.0.0", 8080);
```

Register routes:

```cpp
server.get("/", [](const KupsHTTP::Request &req) {
    KupsHTTP::Response res(200, "OK");
    res.setContent("<h1>Hello</h1>", "text/html");
    return res;
});

server.post("/echo", [](const KupsHTTP::Request &req) {
    KupsHTTP::Response res(200, "OK");
    res.setContent(req.getBody(), req.getHeader("Content-Type"));
    return res;
});
```

Add middleware (logging, auth, etc.) using the router instance:

```cpp
server.getRouter().use([](KupsHTTP::Request &req, KupsHTTP::Response &res) {
    // return true to continue to next middleware/handler, false to short-circuit
    std::cout << "[" << time(nullptr) << "] " << req.getMethodString() << " " << req.getPath() << std::endl;
    return true;
});
```

Start/stop server:

```cpp
server.start(); // blocks or runs event loop depending on implementation
// or run in a background thread depending on your usage
server.stop();
```

## Configuration & notes

- Default host/port are set in the `Server` constructor. Use `setHost()` / `setPort()` to change programmatically.
- The server implementation is intentionally minimal. For production use prefer battle-tested libraries (Boost.Beast, libcpp-httplib, Crow, or full HTTP servers like nginx).
- On macOS you might need to allow binding to low ports (<1024) with elevated privileges.
- If you change headers or public API, update the examples accordingly.

## Troubleshooting

- If CMake can't find a C++17-capable compiler, ensure your system toolchain is up-to-date.
- If port is already in use, change the port in `examples/main.cpp` or configure the Server before `start()`.
- For linker errors, make sure the `include` path is correct and CMake built the `KupsHTTP` target.

## Contributing

Contributions are welcome. Please follow these steps:

1. Open an issue describing the bug/feature.
2. Create a branch for your change.
3. Add tests where applicable.
4. Open a pull request with a description and the rationale.
