cmake_minimum_required(VERSION 3.24)

message(STATUS "Using ${CMAKE_CXX_COMPILER_ID} compiler")

# create project
project(CubbitHorcrux CXX)

set(BOOST_INCLUDE_LIBRARIES system program_options)
set(BOOST_ENABLE_CMAKE ON)

include(FetchContent)
FetchContent_Declare(
  Boost
  GIT_REPOSITORY https://github.com/boostorg/boost.git
  GIT_TAG boost-1.84.0
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(Boost)



# add path the custom find module
#list(APPEND CMAKE_MODULE_PATH ${CubbitHorcrux_SOURCE_DIR}/cmake)

# download and install boost libraries if needed
#set(BOOST_REQUESTED_VERSION 1.84)
#set(BOOST_ROOT_DIR ${CubbitHorcrux_SOURCE_DIR}/externals)

#ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
#find_package(Boost REQUIRED COMPONENTS system log program_options)

add_subdirectory(client)
add_subdirectory(server)