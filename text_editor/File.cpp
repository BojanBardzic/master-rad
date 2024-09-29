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
        {"cp", LanguageMode::Cpp},
        {"java", LanguageMode::Java},
        {"jar", LanguageMode::Java},
        {"jnl", LanguageMode::Java},
        {"c", LanguageMode::C},
        {"h", LanguageMode::C},
        {"cs", LanguageMode::CSharp},
};

bool File::readFromFile(std::string &buffer, const std::string &filePath) {
    std::ifstream input(filePath);

    if (!input.is_open())
        return false;

    try {
        std::getline(input, buffer, '\0');
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    input.close();
    return true;
}

bool File::writeToFile(std::string& buffer, const std::string &filePath) {
    std::string backupBuffer;
    readFromFile(backupBuffer, filePath);

    std::ofstream output(filePath);
    if (!output.is_open())
        return false;

    try {
        output << buffer;
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        output.close();

        std::ofstream backupOutput(filePath);
        backupOutput << backupBuffer;
        backupOutput.close();

        return false;
    }

    output.close();
    return true;
}

std::string File::getWorkingDirectory() {
    char* cwd = _getcwd( 0, 0 ) ; // **** microsoft specific ****
    std::string working_directory(cwd) ;
    std::free(cwd) ;
    return working_directory ;
}

std::string File::getProjectDirectory() {
    std::string workingDirectory = getWorkingDirectory();
    std::string projectName = "\\text_editor\\";
    return workingDirectory.substr(0, workingDirectory.rfind(projectName) + projectName.size());
}
