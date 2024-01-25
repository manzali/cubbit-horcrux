
include_directories(${CubbitHorcrux_SOURCE_DIR}/externals/include)
include_directories(${CubbitHorcrux_SOURCE_DIR}/common/include)

# add executable
add_executable(horcrux main.cpp)

set_property(TARGET horcrux PROPERTY CXX_STANDARD 20)
set_property(TARGET horcrux PROPERTY CXX_STANDARD_REQUIRED On)
set_property(TARGET horcrux PROPERTY CXX_EXTENSIONS Off)
