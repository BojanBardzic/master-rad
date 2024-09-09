//
// Created by bbard on 7/10/2024.
//

#ifndef TEXT_EDITOR_THEME_H
#define TEXT_EDITOR_THEME_H

#include "imgui.h"
#include "../SyntaxHiglighting/TextHighlighter.h"
#include "ThemeName.h"

#include <string>
#include <unordered_map>

enum ThemeColor {
    BackgroundColor,
    TextColor,
    StringColor,
    NumberColor,
    KeywordColor,
    PreprocessorColor,
    CommentColor,
    CursorColor,
    SelectColor,
    WriteSelectColor,
    ScrollbarPrimaryColor,
    ScrollbarSecondaryColor,
};

class Theme {
public:
    Theme(ThemeName name, ImColor backgroundColor, ImColor textColor, ImColor stringColor, ImColor numberColor,
          ImColor keywordColor, ImColor preprocessorColor, ImColor commentColor, ImColor cursorColor, ImColor selectColor,
          ImColor writeSelectColor, ImColor scrollbarPrimaryColor, ImColor scrollbarSecondaryColor);

    ~Theme();

    const ThemeName& getName() const;
    const ImColor& getColor(const ThemeColor& color);
private:
    ThemeName m_name;
    std::unordered_map<ThemeColor, ImColor> m_colors;
};


#endif //TEXT_EDITOR_THEME_H
