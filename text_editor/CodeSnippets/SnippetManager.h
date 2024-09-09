//
// Created by bbard on 9/2/2024.
//

#ifndef TEXT_EDITOR_SNIPPETMANAGER_H
#define TEXT_EDITOR_SNIPPETMANAGER_H

#include "../File.h"

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <set>
#include <unordered_map>

class SnippetManager {
public:
    static void init();

    static std::pair<char**, size_t> getSnippetNames();
    static std::pair<char*, size_t> getTextForName(std::string name);

    static void deleteSnippet(std::string name);
    static bool addSnippet(std::string& name, std::string& text);
private:
    static void insertSnippet(std::string& name, std::string& text);
    static bool saveSnippet(std::string& name, std::string& text);
    static bool checkName(std::string& name);

    static const std::string m_snippetsPath;
    static const std::regex m_nameRegex;
    static std::set<std::string> m_snippetNames;
};


#endif //TEXT_EDITOR_SNIPPETMANAGER_H
