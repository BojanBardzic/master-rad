//
// Created by bbard on 7/10/2024.
//

#include "Theme.h"

Theme::Theme(std::string name, ImColor backgroundColor, ImColor textColor, ImColor stringColor, ImColor numberColor,
             ImColor keywordColor, ImColor cursorColor, ImColor selectColor,
             ImColor scrollbarPrimaryColor, ImColor scrollbarSecondaryColor) : m_name(name) {

    m_colors.insert({ThemeColor::BackgroundColor, backgroundColor});
    m_colors.insert({ThemeColor::TextColor, textColor});
    m_colors.insert({ThemeColor::StringColor, stringColor});
    m_colors.insert({ThemeColor::NumberColor, numberColor});
    m_colors.insert({ThemeColor::KeywordColor, keywordColor});
    m_colors.insert({ThemeColor::CursorColor, cursorColor});
    m_colors.insert({ThemeColor::SelectColor, selectColor});
    m_colors.insert({ThemeColor::ScrollbarPrimaryColor, scrollbarPrimaryColor});
    m_colors.insert({ThemeColor::ScrollbarSecondaryColor, scrollbarSecondaryColor});
}

Theme::~Theme() {}

const std::string &Theme::getName() const { return m_name; }

const ImColor &Theme::getColor(const ThemeColor &color) { return m_colors[color]; }


