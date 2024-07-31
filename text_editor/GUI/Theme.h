//
// Created by bbard on 7/10/2024.
//

#ifndef TEXT_EDITOR_THEME_H
#define TEXT_EDITOR_THEME_H

#include "imgui.h"
#include "TextHighlighter.h"
#include <string>
#include <unordered_map>

enum ThemeColor {
    BackgroundColor,
    TextColor,
    StringColor,
    NumberColor,
    KeywordColor,
    CursorColor,
    SelectColor,
    ScrollbarPrimaryColor,
    ScrollbarSecondaryColor,
};

class Theme {
public:
    Theme(std::string name, ImColor backgroundColor, ImColor textColor, ImColor stringColor, ImColor numberColor,
          ImColor keywordColor, ImColor cursorColor, ImColor selectColor,
          ImColor scrollbarPrimaryColor, ImColor scrollbarSecondaryColor);

    ~Theme();

    const std::string& getName() const;
    const ImColor& getColor(const ThemeColor& color);
private:
    std::string m_name;
    std::unordered_map<ThemeColor, ImColor> m_colors;
};


#endif //TEXT_EDITOR_THEME_H
