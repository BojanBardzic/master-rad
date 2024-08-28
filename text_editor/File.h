//
// Created by bbard on 7/21/2024.
//

#ifndef TEXT_EDITOR_FILE_H
#define TEXT_EDITOR_FILE_H

#include <string>
#include <unordered_map>
#include "SyntaxHiglighting/LanguageMode.h"

class File {
public:
    File(std::string filePath);
    ~File();

    const std::string& getPath() const;
    const std::string& getName() const;
    const std::string& getExtension() const;

    static LanguageMode getModeForExtension(const std::string& extension);
private:
    std::string m_path;
    std::string m_name;
    std::string m_extension;

    const static std::unordered_map<std::string, LanguageMode> m_extensions;
};


#endif //TEXT_EDITOR_FILE_H
