set(target horcrux)

# add executable
add_executable(${target} main.cpp)

target_include_directories(
    ${target} PUBLIC
    ${CubbitHorcrux_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set_property(TARGET ${target} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${target} PROPERTY CXX_STANDARD_REQUIRED On)
set_property(TARGET ${target} PROPERTY CXX_EXTENSIONS Off)

target_link_libraries(
    ${target} PRIVATE
    Boost::asio
    Boost::beast
    Boost::uuid
    Boost::json
)