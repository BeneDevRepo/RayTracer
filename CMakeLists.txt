cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_CXX_STANDARD 20)

project(Client)
add_compile_options(-O2)
add_compile_definitions(UNICODE)

# include(CTest)
# enable_testing()

file(GLOB_RECURSE SOURCES "src/*.cpp") # All c++ files

# -- Main Executable --
add_executable(Client main.cpp ${SOURCES})
target_include_directories(Client PUBLIC "src")
target_link_libraries(Client "ws2_32.lib")
target_link_libraries(Client "winmm.lib")
# target_link_libraries(Client "libopengl32.lib")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)