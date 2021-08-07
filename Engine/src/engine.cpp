#include "engine/engine.hpp"

#include <GLFW/glfw3.h>

#include "log_internal.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "engine/gui/font/ascii_font.hpp"

#include "engine/log.hpp"

namespace engine {

namespace gui::font {
void initAsciiIndexes();
}

void init() {
  gui::font::initAsciiIndexes();

  logging::setupLogging();

  glfwInit();

}

void terminate() {
  glfwTerminate();

  logging::terminateLogging();
}

}   // namespace engine