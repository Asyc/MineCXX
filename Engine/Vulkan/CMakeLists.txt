find_package(Vulkan REQUIRED)

set(HEADERS
        include/engine/vulkan/vulkan_context.hpp
        include/engine/vulkan/device/vulkan_queue.hpp
        include/engine/vulkan/render/vulkan_pipeline.hpp
        include/engine/vulkan/render/command/vulkan_command_buffer.hpp
        include/engine/vulkan/render/command/vulkan_command_pool.hpp
        include/engine/vulkan/render/swapchain/vulkan_image.hpp
        include/engine/vulkan/render/swapchain/vulkan_swapchain.hpp
        include/engine/vulkan/render/buffer/vulkan_vertex_buffer.hpp
        include/engine/vulkan/render/buffer/vulkan_index_buffer.hpp
        include/engine/vulkan/render/buffer/vulkan_image.hpp
        include/engine/vulkan/render/buffer/vulkan_uniform_buffer.hpp
        include/engine/vulkan/device/vulkan_device.hpp
        include/engine/vulkan/render/vulkan_descriptor.hpp
        )

set(SOURCES
        src/vulkan_context.cpp
        src/render/vulkan_pipeline.cpp
        src/render/vulkan_program.cpp
        src/render/command/vulkan_command_buffer.cpp
        src/render/command/vulkan_command_pool.cpp src/render/swapchain/vulkan_image.cpp
        src/render/swapchain/vulkan_swapchain.cpp
        src/render/buffer/vulkan_transfer_pool.cpp
        src/render/buffer/vulkan_vertex_buffer.cpp
        src/render/buffer/vulkan_index_buffer.cpp
        src/render/buffer/vulkan_image.cpp
        src/render/buffer/vulkan_uniform_buffer.cpp
        src/device/vulkan_device.cpp src/device/vulkan_queue.cpp
        src/render/vulkan_descriptor.cpp
        )

add_library(libvk ${HEADERS} ${SOURCES})
target_link_libraries(libvk PUBLIC
        Vulkan::Vulkan
        VulkanMemoryAllocator
        SPIRV
        spirv-cross-cpp
        glslang
        stb
        libengine
        )


target_include_directories(libvk PUBLIC include)
target_include_directories(libvk PRIVATE src)

add_compile_definitions(VULKAN_HPP_NO_NODISCARD_WARNINGS)