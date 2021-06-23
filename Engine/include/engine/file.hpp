#ifndef MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_FILE_HPP_
#define MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_FILE_HPP_

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace engine {

class File {
public:
    File(std::string path);

    [[nodiscard]] std::vector<char> readFileBinary() const;
    [[nodiscard]] std::string readFileText() const;

    [[nodiscard]] std::filesystem::directory_iterator getDirectoryIterator() const {
        return std::filesystem::directory_iterator(m_Path);
    }

    bool exists() const;
    const std::string& getPath() const;
    const std::string& getFullPath() const;
    const std::string& getParentPath() const;
private:
    std::string m_Path;
    mutable std::optional<std::string> m_FullPath, m_ParentPath;
};

}

#endif //MINECRAFTCXX_CLIENT_ENGINE_INCLUDE_ENGINE_FILE_HPP_
