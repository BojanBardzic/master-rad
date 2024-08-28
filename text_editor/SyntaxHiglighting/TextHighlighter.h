//
// Created by bbard on 7/28/2024.
//

#ifndef TEXT_EDITOR_TEXTHIGHLIGHTER_H
#define TEXT_EDITOR_TEXTHIGHLIGHTER_H

#include <iostream>
#include <regex>
#include <set>
#include <sstream>
#include <unordered_map>
#include "../GUI/Theme.h"
#include "lexertk.hpp"
#include "LanguageManager.h"

enum ThemeColor;

class TextHighlighter {
public:
    static std::vector<ThemeColor> getColorMap(std::string& line, LanguageMode mode);
private:
    static void searchRegex(std::string line, std::vector<ThemeColor>& colorMap, const std::regex& regex, ThemeColor color);
    static void searchForKeywordsAndNumbers(std::string line, std::vector<ThemeColor>& colorMap, LanguageMode mode);
    static void parseSegment(std::string& line, size_t start, size_t end, std::vector<ThemeColor>& colorMap, LanguageMode mode);
    static void searchForPreprocessorCommands(std::string& line, std::vector<ThemeColor>& colorMap);
    static void searchForSingleLineComment(std::string& line, std::vector<ThemeColor>& colorMap, LanguageMode mode);

    static const std::regex m_stringRegex;
    static const std::regex m_numberRegex;
    static lexertk::generator generator;
};


#endif //TEXT_EDITOR_TEXTHIGHLIGHTER_H
