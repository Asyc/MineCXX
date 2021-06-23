find_package(Vulkan REQUIRED)

set(BINARY libvk)

add_library(libvk
        src/vkr.cpp
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
        )

target_include_directories(libvk PUBLIC include)
target_include_directories(libvk PRIVATE src)

target_link_libraries(libvk PUBLIC Vulkan::Vulkan libengine)

add_subdirectory(${PROJECT_SOURCE_DIR}/vendors/spirv-cross ${PROJECT_SOURCE_DIR}/cmake-build-debug/vendors/spirv-cross)

target_link_libraries(libvk PUBLIC spirv-cross-cpp)

## -- Testing -- ##
set(TEST_SOURCES tests/program_test.cpp)

add_executable(libvk_test ${TEST_SOURCES} include/engine/vulkan/render/swapchain/vulkan_swapchain.hpp)

add_test(NAME libvk_test COMMAND libvk_test)

target_link_libraries(libvk_test PUBLIC gtest_main libvk)
