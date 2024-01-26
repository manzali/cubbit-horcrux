cmake_minimum_required(VERSION 3.24)

message(STATUS "Using ${CMAKE_CXX_COMPILER_ID} compiler")

# create project
project(CubbitHorcrux CXX)

set(BOOST_INCLUDE_LIBRARIES asio system thread regex json program_options uuid)
set(BOOST_ENABLE_CMAKE ON)

include(FetchContent)
FetchContent_Declare(
  Boost
  GIT_REPOSITORY https://github.com/boostorg/boost.git
  GIT_TAG boost-1.84.0
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(Boost)

add_subdirectory(client)
add_subdirectory(server)