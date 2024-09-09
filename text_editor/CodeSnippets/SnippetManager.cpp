//
// Created by bbard on 9/2/2024.
//

#include "SnippetManager.h"

const std::string SnippetManager::m_snippetsPath = File::getProjectDirectory() + "Snippets\\";
const std::regex SnippetManager::m_nameRegex = std::regex("^[\\w\\-. ]+$");
std::set<std::string> SnippetManager::m_snippetNames;

void SnippetManager::init() {
    try {
        for (const auto &entry: std::filesystem::directory_iterator(m_snippetsPath)) {
            if (entry.is_regular_file()) {
                std::string name = entry.path().stem().string();
                m_snippetNames.insert(name);
            }
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}

std::pair<char**, size_t> SnippetManager::getSnippetNames() {
    char** result = new char*[m_snippetNames.size()];

    size_t i = 0;
    for (auto name : m_snippetNames) {
        result[i] = new char[name.size()+1];
        result[i][name.size()] = '\0';
        std::copy(name.begin(), name.end(), result[i]);
        ++i;
    }

    return {result, m_snippetNames.size()};
}

std::pair<char*, size_t> SnippetManager::getTextForName(std::string name) {
    std::string text;
    std::string path = m_snippetsPath + name + ".txt";
    File::readFromFile(text, path);

    char* result = new char[text.size()+1];
    result[text.size()] = '\0';
    std::copy(text.begin(), text.end(), result);

    return {result, text.size()+1};
}

void SnippetManager::deleteSnippet(std::string name) {
    auto path = m_snippetsPath + name + ".txt";
    std::remove(path.c_str());
    m_snippetNames.erase(name);
}

bool SnippetManager::addSnippet(std::string &name, std::string &text) {
    if (checkName(name)) {
        insertSnippet(name, text);
        saveSnippet(name, text);

        return true;
    }

    return false;
}

void SnippetManager::insertSnippet(std::string &name, std::string &text) {
    m_snippetNames.insert(name);
}

bool SnippetManager::saveSnippet(std::string &name, std::string &text) {
    std::string path = m_snippetsPath + name + ".txt";
    return File::writeToFile(text, path);
}

bool SnippetManager::checkName(std::string &name) {
    if (!std::regex_match(name, m_nameRegex))
        return false;

    return m_snippetNames.find(name) == m_snippetNames.end();
}



