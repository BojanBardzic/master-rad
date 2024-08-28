//
// Created by bbard on 7/28/2024.
//

#include "TextHighlighter.h"

const std::regex TextHighlighter::m_stringRegex = std::regex(R"((['"])((\\\1|.)*?)\1)");
const std::regex TextHighlighter::m_numberRegex = std::regex(R"(([1-9]\d*|0|(\.\d+))(\.\d+)?)");
lexertk::generator TextHighlighter::generator;

std::vector<ThemeColor> TextHighlighter::getColorMap(std::string& line, LanguageMode mode) {
    std::vector<ThemeColor> colorMap(line.size(), ThemeColor::TextColor);

    if (LanguageManager::getLanguage(mode)->isPreprocessor())
        searchForPreprocessorCommands(line, colorMap);

    searchForSingleLineComment(line, colorMap, mode);
    searchRegex(line, colorMap, m_stringRegex, ThemeColor::StringColor);
    searchForKeywordsAndNumbers(line, colorMap, mode);

    return colorMap;
}

void TextHighlighter::searchRegex(std::string line, std::vector<ThemeColor> &colorMap, const std::regex &regex, ThemeColor color) {
    std::smatch regexMatch;
    size_t start = 0;
    size_t matchSize = 0;

    while (std::regex_search(line, regexMatch, regex)) {
        start += regexMatch.prefix().length();
        matchSize = regexMatch[0].length();

        if (colorMap[start] == ThemeColor::TextColor)
            std::fill(colorMap.begin() + start, colorMap.begin() + start + matchSize, color);

        start += matchSize;
        line = regexMatch.suffix();
    }
}

void TextHighlighter::searchForKeywordsAndNumbers(std::string line, std::vector<ThemeColor> &colorMap, LanguageMode mode) {
    std::stringstream stream(line);

    size_t start = 0;

    while (start < line.size()) {
        if (colorMap[start] == ThemeColor::TextColor) {
            size_t end = start;

            while (end+1 < colorMap.size() && colorMap[end+1] == ThemeColor::TextColor) {
                ++end;
            }

            parseSegment(line, start, end, colorMap, mode);

            start = end+1;
        } else {
            ++start;
        }
    }
}

void TextHighlighter::parseSegment(std::string& line, size_t start, size_t end, std::vector<ThemeColor>& colorMap, LanguageMode mode) {
    auto keywords = LanguageManager::getLanguage(mode)->getKeywords();
    auto segment = line.substr(start, end-start + 1);
    if (generator.process(segment)) {
        for (size_t i=0; i<generator.size(); ++i) {
            auto token = generator[i];
            size_t position = token.position;
            size_t length = token.value.size();

            if (token.type == lexertk::token::e_symbol && keywords.find(token.value) != keywords.end()) {
                std::fill(colorMap.begin() + start + position, colorMap.begin() + start + position + length, ThemeColor::KeywordColor);
            } else if (token.value != "." && token.type == lexertk::token::e_number) {
                std::fill(colorMap.begin() + start + position, colorMap.begin() + start + position + length, ThemeColor::NumberColor);
            }
        }
    }
}

void TextHighlighter::searchForPreprocessorCommands(std::string &line, std::vector<ThemeColor> &colorMap) {
    if (!line.empty() && line[0] == '#')
        std::fill(colorMap.begin(), colorMap.end(), ThemeColor::PreprocessorColor);
}

void TextHighlighter::searchForSingleLineComment(std::string& line, std::vector<ThemeColor>& colorMap, LanguageMode mode) {
    auto start = line.find(LanguageManager::getLanguage(mode)->getSingleLineCommentStart());

    if (start != std::string::npos) {
        std::fill(colorMap.begin() + start, colorMap.end(), ThemeColor::CommentColor);
    }
}
