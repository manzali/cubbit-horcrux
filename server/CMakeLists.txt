include_directories(
  ${CubbitHorcrux_SOURCE_DIR}/common/include
)

# add executable
add_executable(horcrux-server main.cpp)

set_property(TARGET horcrux-server PROPERTY CXX_STANDARD 20)
set_property(TARGET horcrux-server PROPERTY CXX_STANDARD_REQUIRED On)
set_property(TARGET horcrux-server PROPERTY CXX_EXTENSIONS Off)

target_link_libraries(horcrux PRIVATE Boost::asio Boost::json Boost::program_options)