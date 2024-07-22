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
