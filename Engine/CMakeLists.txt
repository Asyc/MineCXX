find_package(Vulkan)

set(HEADERS
        include/engine/engine.hpp
        include/engine/file.hpp
        include/engine/window.hpp
        include/engine/log.hpp
        include/engine/render/render_context.hpp
        include/engine/render/swapchain.hpp
        include/engine/render/pipeline.hpp
        include/engine/render/command/command_pool.hpp
        include/engine/render/command/command_buffer.hpp
        include/engine/render/buffer/vertex_buffer.hpp
        include/engine/render/buffer/index_buffer.hpp
        include/engine/gui/gui.hpp include/engine/gui/element/element.hpp include/engine/gui/input.hpp include/engine/render/buffer/image.hpp include/engine/render/buffer/uniform_buffer.hpp)

set(SOURCES
        src/engine.cpp
        src/window.cpp
        src/log.cpp
        src/log_internal.hpp
        src/pipeline.cpp
        src/file.cpp
        src/gui/gui.cpp)

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

