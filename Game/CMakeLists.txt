add_executable(Minecraft src/main.cpp)

target_link_libraries(Minecraft PUBLIC libengine)
target_include_directories(Minecraft PUBLIC include)
target_include_directories(Minecraft PRIVATE src)