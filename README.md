# Client
Minecraft Clone written in C++ currently running Vulkan.

Features:
* Abstract Game Engine Design (Swapable Graphical API, SPIR-V Cross-Compilation)
* Instanced Chunk Rendering
* Texture Atlas Generation
* Recreation GUI System (Incomplete)
* Geometry Shader Font Rendering
* Named Binary Tag Manipulation
  
Modules:
```
Engine/    # Vulkan Game Engine
Game/      # Minecraft Clone Implementation
```

Dependencies (as submodules):
* rectpack2D
* glslang
* glm
* glfw
* rapidjson
* spdlog
* spirv-cross
* stb
* vma
* google-test
