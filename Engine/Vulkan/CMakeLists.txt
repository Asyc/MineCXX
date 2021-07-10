find_package(Vulkan REQUIRED)

set(BINARY libvk)

add_library(libvk
        include/engine/vulkan/vulkan_context.hpp
        src/vulkan_context.cpp
        include/engine/vulkan/vulkan_queue.hpp
        include/engine/vulkan/render/vulkan_pipeline.hpp
        src/render/vulkan_pipeline.cpp
        src/render/vulkan_program.cpp
        include/engine/vulkan/render/command/vulkan_command_buffer.hpp
        include/engine/vulkan/render/command/vulkan_command_pool.hpp
        src/render/command/vulkan_command_buffer.cpp
        src/render/command/vulkan_command_pool.cpp src/render/swapchain/vulkan_image.cpp
        include/engine/vulkan/render/swapchain/vulkan_image.hpp
        include/engine/vulkan/render/swapchain/vulkan_swapchain.hpp
        src/render/swapchain/vulkan_swapchain.cpp
        src/render/buffer/vulkan_transfer_pool.cpp
        include/engine/vulkan/render/buffer/vulkan_vertex_buffer.hpp
        src/render/buffer/vulkan_vertex_buffer.cpp include/engine/vulkan/vulkan_resource.hpp
        src/vulkan_resource.cpp include/engine/vulkan/render/buffer/vulkan_index_buffer.hpp
        src/render/buffer/vulkan_index_buffer.cpp
        )

target_include_directories(libvk PUBLIC include)
target_include_directories(libvk PRIVATE src)

target_link_libraries(libvk PUBLIC Vulkan::Vulkan SPIRV glslang spirv-cross-cpp libengine)

add_compile_definitions(VULKAN_HPP_NO_NODISCARD_WARNINGS)