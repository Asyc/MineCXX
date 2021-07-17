#include "engine/engine.hpp"

#include <GLFW/glfw3.h>

#include "log_internal.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace engine {

void init() {
    glfwInit();

    logging::setupLogging();
}

void terminate() {
    glfwTerminate();

    logging::terminateLogging();
}

}   // namespace engine