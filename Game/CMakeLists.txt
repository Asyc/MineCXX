add_executable(Minecraft src/main.cpp src/gui/main_menu.hpp src/gui/main_menu.cpp src/world/chunk.cpp src/world/chunk.hpp src/world/block.cpp src/world/block.hpp ../Engine/include/engine/world_object.hpp src/world/world.cpp src/world/world.hpp)

target_link_libraries(Minecraft PUBLIC libengine)
target_include_directories(Minecraft PUBLIC include)
target_include_directories(Minecraft PRIVATE src)