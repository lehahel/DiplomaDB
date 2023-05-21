cmake_minimum_required(VERSION 3.12)
project(application-example)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(contrib/clickhouse-cpp)
add_subdirectory(contrib/fmt)

find_package(Boost 1.68 REQUIRED)
add_executable(${PROJECT_NAME} "src/main.cpp" "src/graph.cpp")

target_include_directories(${PROJECT_NAME} PRIVATE contrib/clickhouse-cpp/ contrib/clickhouse-cpp/contrib/absl)
target_include_directories(${PROJECT_NAME} PRIVATE contrib/fmt)

target_link_libraries(${PROJECT_NAME} PRIVATE clickhouse-cpp-lib fmt::fmt)
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::boost)
