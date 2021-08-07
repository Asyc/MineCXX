cmake_minimum_required(VERSION 3.8.0)
project(MinecraftCXX_Client)

set(CMAKE_CXX_STANDARD 17)

# Adding All Subdirectory Libraries
add_subdirectory(vendors/spirv-cross)
add_subdirectory(vendors/glslang)
add_subdirectory(vendors/glfw)
add_subdirectory(vendors/glm)
add_subdirectory(vendors/spdlog)

# Create a custom target for RapidJSON library
add_library(rapidjson INTERFACE)
target_include_directories(rapidjson INTERFACE vendors/rapidjson/include)

add_library(rectpack2D INTERFACE)
target_include_directories(rectpack2D INTERFACE vendors/rectpack/src)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE vendors/stb)

option(VMA_STATIC_VULKAN_FUNCTIONS "Link statically with Vulkan API" OFF)
option(VMA_DYNAMIC_VULKAN_FUNCTIONS "Fetch pointers to Vulkan functions internally (no static linking)" OFF)
add_subdirectory(vendors/vma)
 
add_subdirectory(vendors/googletest)

add_subdirectory(Engine)
add_subdirectory(Game)