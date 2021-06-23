find_package(Vulkan)

set(HEADERS
        include/engine/engine.hpp
        include/engine/window.hpp
        include/engine/log.hpp
        include/engine/render/render_context.hpp
        )

set(SOURCES
        src/engine.cpp
        src/window.cpp
        src/log.cpp
        src/log_internal.hpp
        include/engine/render/swapchain.hpp include/engine/render/pipeline.hpp src/pipeline.cpp include/engine/file.hpp src/file.cpp include/engine/render/command/command_pool.hpp include/engine/render/command/command_buffer.hpp)

add_library(libengine STATIC ${HEADERS} ${SOURCES})

if(Vulkan_FOUND)
    message("-- Linking Vulkan Module")
    target_compile_definitions(libengine PUBLIC MCE_VK)

    add_subdirectory(Vulkan)
    target_link_libraries(libengine PUBLIC libvk)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(libengine PUBLIC MCE_DBG)
endif()

target_include_directories(libengine PUBLIC include)
target_include_directories(libengine PRIVATE src)

add_subdirectory(vendors/glfw)
add_subdirectory(vendors/spdlog)

target_link_libraries(libengine PRIVATE glfw)
target_link_libraries(libengine PUBLIC rapidjson spdlog)

