//
// Created by bbard on 7/21/2024.
//

#ifndef TEXT_EDITOR_FILE_H
#define TEXT_EDITOR_FILE_H

#include "SyntaxHiglighting/LanguageMode.h"

#include <fstream>
#include <iostream>
#include <string>
#include <direct.h>
#include <unordered_map>

class File {
public:
    explicit File(std::string filePath);
    ~File();

    const std::string& getPath() const;
    const std::string& getName() const;
    const std::string& getExtension() const;

    static LanguageMode getModeForExtension(const std::string& extension);
    static bool readFromFile(std::string& buffer, const std::string& filePath);
    static bool writeToFile(std::string& buffer, const std::string& filePath);
    static std::string getWorkingDirectory();
    static std::string getProjectDirectory();
private:
    std::string m_path;
    std::string m_name;
    std::string m_extension;

    const static std::unordered_map<std::string, LanguageMode> m_extensions;
};


#endif //TEXT_EDITOR_FILE_H
