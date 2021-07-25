add_executable(Minecraft src/main.cpp)

target_link_libraries(Minecraft PUBLIC libengine)
target_include_directories(Minecraft PUBLIC include)
target_include_directories(Minecraft PRIVATE src)

add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
)
add_dependencies(Minecraft copy_assets)

set_target_properties(Minecraft PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")