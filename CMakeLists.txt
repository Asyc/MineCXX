cmake_minimum_required(VERSION 3.19)
project(MinecraftCXX_Client)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(vendors/glslang)
add_subdirectory(vendors/rapidjson)

# Create a custom target for RapidJSON library
add_library(rapidjson INTERFACE)
target_include_directories(rapidjson INTERFACE vendors/rapidjson/include)

add_subdirectory(vendors/googletest)

add_subdirectory(ProgramC)
add_subdirectory(Engine)
add_subdirectory(Game)