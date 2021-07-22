#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_FILE_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_FILE_HPP_

#include <filesystem>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace engine {

class File {
public:
    File(std::string path);

    void deleteFile();

    [[nodiscard]] std::vector<char> readFileBinary() const;
    [[nodiscard]] std::string readFileText() const;

    virtual bool exists() const { return std::filesystem::exists(m_Path); }
    virtual const std::string& getPath() const { return m_Path; }
    virtual const std::string& getFullPath() const;
    virtual const std::string& getParentPath() const;
protected:
    std::string m_Path;
    mutable std::optional<std::string> m_FullPath, m_ParentPath;
};

class Directory {
public:
    Directory(std::string path) : m_Path(std::move(path)) {}

    [[nodiscard]] std::filesystem::directory_iterator getDirectoryIterator() const {
        return std::filesystem::directory_iterator(m_Path);
    }

    virtual const std::string& getPath() const { return m_Path; }
    virtual const std::string& getFullPath() const;
    virtual const std::string& getParentPath() const;
protected:
    std::string m_Path;
    mutable std::optional<std::string> m_FullPath, m_ParentPath;
};

}

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_FILE_HPP_
