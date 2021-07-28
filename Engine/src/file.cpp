#include "engine/file.hpp"

#include <fstream>
#include <filesystem>

namespace engine {

File::File(std::string path) : m_Path(std::move(path)) {}

void File::deleteFile() {
    if (!std::filesystem::remove(m_Path))
        throw std::runtime_error("failed to delete file");
}

void File::write(const void* data, size_t length) const {
    std::ofstream stream(m_Path, std::ios_base::binary);
    stream.write(reinterpret_cast<const char*>(data), length);
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

const std::string& Directory::getFullPath() const {
    if (!m_FullPath.has_value()) {
        m_FullPath = std::filesystem::absolute(m_Path).string();
    }

    return *m_FullPath;
}
const std::string& Directory::getParentPath() const {
    if (!m_ParentPath.has_value()) {
        m_ParentPath = std::filesystem::absolute(m_Path).parent_path().string();
    }

    return *m_ParentPath;
}

std::string concat(const std::string_view& ns, const std::string_view& path) {
    return "assets/" + std::string(ns) + "/" + std::string(path);
}

NamespaceFile::NamespaceFile(const std::string_view& pNamespace, const std::string_view& path) : File(concat(pNamespace, path)) {

}

}   // namespace engine