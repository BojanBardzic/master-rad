//
// Created by bbard on 7/21/2024.
//

#include "File.h"

File::File(std::string filePath) : m_path(filePath) {
    m_name = filePath.substr(filePath.find_last_of("\\\\") + 1);

    auto index = filePath.find_last_of('.');
    m_extension = index != std::string::npos ? filePath.substr(index + 1) : "";
}

File::~File() {}

const std::string &File::getPath() const { return m_path; }

const std::string &File::getName() const { return m_name; }

const std::string &File::getExtension() const { return m_extension; }

LanguageMode File::getModeForExtension(const std::string &extension) {
    if (m_extensions.find(extension) != m_extensions.end())
        return m_extensions.at(extension);
    else
        return LanguageMode::PlainText;
}

const std::unordered_map<std::string, LanguageMode> File::m_extensions = {
        {"cpp", LanguageMode::Cpp},
        {"cxx", LanguageMode::Cpp},
        {"hpp", LanguageMode::Cpp},
        {"C", LanguageMode::Cpp},
        {"cc", LanguageMode::Cpp},
        {"cp", LanguageMode::Cpp}
};
