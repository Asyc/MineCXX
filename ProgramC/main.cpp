#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <rapidjson/document.h>

#include <glslang/Include/glslang_c_interface.h>

template<glslang_stage_t STAGE>
std::vector<char> createSPIRV(std::string src) {
    const glslang_input_t input{
        .language = GLSLANG_SOURCE_GLSL,
        .stage = STAGE,
        .client = GLSLANG_CLIENT_VULKAN,
        .client_version = GLSLANG_TARGET_VULKAN_1_2,
        .target_language = GLSLANG_TARGET_SPV,
        .target_language_version = GLSLANG_TARGET_SPV_1_5,
        .code = src.data(),
        .default_version = 100,
        .default_profile = GLSLANG_CORE_PROFILE,
        .force_default_version_and_profile = false,
        .forward_compatible = false,
        .messages = GLSLANG_MSG_DEFAULT_BIT,
    };

    glslang_initialize_process();
    auto* shader = glslang_shader_create(&input);

    if (!glslang_shader_preprocess(shader, &input)) {
        std::cerr << glslang_shader_get_info_log(shader) << '\n' << glslang_shader_get_info_debug_log(shader);
        return {};
    }

    if (!glslang_shader_parse(shader, &input)) {
        std::cerr << glslang_shader_get_info_log(shader) << '\n' << glslang_shader_get_info_debug_log(shader);
        return {};
    }

    glslang_program_t* program = glslang_program_create();
    glslang_program_add_shader(program, shader);

    if (!glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT)) {
        std::cerr << glslang_program_get_info_log(program) << '\n' << glslang_program_get_info_debug_log(program);
        return {};
    }

    glslang_program_SPIRV_generate(program, input.stage);

    if (glslang_program_SPIRV_get_messages(program)) {
        printf("%s", glslang_program_SPIRV_get_messages(program));
    }

    glslang_shader_delete(shader);

    auto* ptr = glslang_program_SPIRV_get_ptr(program);
    size_t size = glslang_program_SPIRV_get_size(program);

    std::vector<char> buffer(size * sizeof(unsigned int));
    memcpy(buffer.data(), ptr, buffer.size());

    glslang_program_delete(program);
    return std::move(buffer);
}

std::string readFile(const std::string_view& path) {
    std::ifstream in(path.data());

    std::string buffer;
    buffer.assign((std::istreambuf_iterator<char>(in)),
                  std::istreambuf_iterator<char>());

    return std::move(buffer);
}

bool writeFile(const std::string_view& path, void* buffer, size_t length) {
    std::ofstream out(path.data());
    out.write(static_cast<const char*>(buffer), static_cast<std::streamsize>(length));
    out.close();
    if (!out) {
        std::cerr << "Failed to write to file " << path << '\n';
        return false;
    }

    return true;
}

bool processProgram(const std::string& json) {
    rapidjson::Document document;
    document.Parse(json.c_str(), json.length());

    auto vulkan = document["vulkan"].GetObject();
    std::string vertexSPV(vulkan["vertexPath"].GetString());
    std::string vertexGLSL = vertexSPV.substr(0, vertexSPV.size() - 4) + ".vert";

    auto vertexBuffer = createSPIRV<GLSLANG_STAGE_VERTEX>(readFile(vertexSPV));
    if (vertexBuffer.empty()) {
        std::cerr << "Failed to compile vertex shader.\n";
        return false;
    }

    if (!writeFile(vertexSPV, vertexBuffer.data(), vertexBuffer.size())) return false;

    std::string fragmentSPV(vulkan["fragmentPath"].GetString());
    std::string fragmentGLSL = vertexSPV.substr(0, vertexSPV.size() - 4) + ".frag";

    auto fragmentBuffer = createSPIRV<GLSLANG_STAGE_FRAGMENT>(readFile(fragmentSPV));
    if (fragmentBuffer.empty()) {
        std::cerr << "Failed to compile fragment shader.\n";
        return false;
    }

    if (!writeFile(fragmentSPV, fragmentBuffer.data(), fragmentBuffer.size())) return false;

    return true;
}

int main(int argc, const char** argv) {
    if (argc <= 1) {
        std::cerr << "Not enough parameters.\n";
        return -1;
    }

    for (int i = 1; i < argc; i++) {
        if (!processProgram(readFile(std::string(argv[i]) + "/config.json"))) {
            return -1;
        }

        std::cout << "Compiled " << argv[i] << '\n';
    }

    return 0;
}