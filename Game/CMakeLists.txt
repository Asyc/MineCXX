add_executable(Minecraft src/main.cpp src/gui/main_menu.hpp src/gui/main_menu.cpp)

target_link_libraries(Minecraft PUBLIC libengine)
target_include_directories(Minecraft PUBLIC include)
target_include_directories(Minecraft PRIVATE src)

set_target_properties(Minecraft PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")