#include "engine/engine.hpp"

#include <GLFW/glfw3.h>

#include "log_internal.hpp"

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