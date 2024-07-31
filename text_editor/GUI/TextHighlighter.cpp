//
// Created by bbard on 7/28/2024.
//

#include "TextHighlighter.h"

const std::regex TextHighlighter::m_stringRegex = std::regex(R"((['"])((\\\1|.)*?)\1)");
const std::regex TextHighlighter::m_numberRegex = std::regex(R"(([1-9]\d*|0|(\.\d+))(\.\d+)?)");
lexertk::generator TextHighlighter::generator;

const std::unordered_map<LanguageMode, std::set<std::string>*> TextHighlighter::m_keywords = {
        {
            LanguageMode::Cpp,
            new std::set<std::string> {
            "alignas", "alignof", "and", "and_eq", "asm",
            "atomic_cancel", "atomic_commit", "atomic_noexcept",
            "auto", "bitand", "bitor", "bool", "break", "case",
            "catch", "char", "char8_t", "char16_t", "char32_t",
            "class", "compl", "concept", "const", "consteval",
            "constexpr", "constinit", "const_cast", "continue",
            "co_await", "co_return", "co_yield", "decltype",
            "default", "delete", "do", "double", "dynamic_cast",
            "else", "enum", "explicit", "export", "extern",
            "false", "float", "for", "friend", "goto", "if",
            "inline", "int", "long", "mutable", "namespace",
            "new", "noexcept", "not", "not_eq", "nullptr",
            "operator", "or", "or_eq", "private", "protected",
            "public", "reflexpr", "register", "reinterpret_cast",
            "requires", "return", "short", "signed", "sizeof",
            "static", "static_assert", "static_cast", "struct",
            "switch", "synchronized", "template", "this",
            "thread_local", "throw", "true", "try", "typedef",
            "typeid", "typename", "union", "unsigned", "using",
            "virtual", "void", "volatile", "wchar_t", "while",
            "xor", "xor_eq"
            }
        }
};


std::vector<ThemeColor> TextHighlighter::getColorMap(std::string& line, LanguageMode mode) {
    std::vector<ThemeColor> colorMap(line.size(), ThemeColor::TextColor);

    searchRegex(line, colorMap, m_stringRegex, ThemeColor::StringColor);
    searchRegex(line, colorMap, m_numberRegex, ThemeColor::NumberColor);
    std::cerr << "Entered search for keywords" << std::endl;
    searchForKeywords(line, colorMap, mode);
    std::cerr << "Exited search for keywords" << std::endl;

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

void TextHighlighter::searchForKeywords(std::string line, std::vector<ThemeColor> &colorMap, LanguageMode mode) {
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
    std::set<std::string>* keywords = m_keywords.at(mode);
    auto segment = line.substr(start, end-start + 1);
    if (generator.process(segment)) {
        for (size_t i=0; i<generator.size(); ++i) {
            auto token = generator[i];

            if (token.type == lexertk::token::e_symbol && keywords->find(token.value) != keywords->end()) {
                std::cerr << "Symbol: " << token.value << std::endl;
                size_t position = token.position;
                size_t length = token.value.size();
                std::fill(colorMap.begin() + start + position, colorMap.begin() + start + position + length, ThemeColor::KeywordColor);
            }
        }
    }
}
