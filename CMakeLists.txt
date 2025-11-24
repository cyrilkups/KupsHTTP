cmake_minimum_required(VERSION 3.10)
project(KupsHTTP VERSION 1.0)

# Set C++ standard to C++17 (required for std::filesystem)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add source files
set(SOURCES
    src/server.cpp
    src/request.cpp
    src/response.cpp
    src/router.cpp
)

# Create library
add_library(${PROJECT_NAME} STATIC ${SOURCES})

# Add compile options
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()

# Add threading support
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

# Create example executable
add_executable(${PROJECT_NAME}_example examples/main.cpp)
target_link_libraries(${PROJECT_NAME}_example PRIVATE ${PROJECT_NAME})

# Install rules
install(TARGETS ${PROJECT_NAME}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(DIRECTORY include/KupsHTTP
        DESTINATION include
        FILES_MATCHING PATTERN "*.h")