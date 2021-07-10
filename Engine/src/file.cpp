#include "engine/file.hpp"

#include <fstream>
#include <filesystem>

namespace engine {

File::File(std::string path) : m_Path(std::move(path)) {}

void File::deleteFile() {
    if (!std::filesystem::remove(m_Path))
        throw std::runtime_error("failed to delete file");
}

std::vector<char> File::readFileBinary() const {
    std::ifstream in(m_Path, std::ios_base::binary | std::ios_base::ate);
    auto pos = in.tellg();
    in.seekg(0);

    std::vector<char> buffer(pos);
    in.read(buffer.data(), pos);
    return std::move(buffer);
}

std::string File::readFileText() const {
    std::ifstream in(m_Path);

    std::string buffer;
    buffer.assign((std::istreambuf_iterator<char>(in)),
               std::istreambuf_iterator<char>());

    return std::move(buffer);
}

bool File::exists() const {
    return std::filesystem::exists(m_Path);
}

const std::string& File::getPath() const {
    return m_Path;
}

const std::string& File::getFullPath() const {
    if (!m_FullPath.has_value()) {
        m_FullPath = std::filesystem::absolute(m_Path).string();
    }

    return *m_FullPath;
}

const std::string& File::getParentPath() const {
    if (!m_ParentPath.has_value()) {
        m_ParentPath = std::filesystem::absolute(m_Path).parent_path().string();
    }

    return *m_ParentPath;
}

}   // namespace engine