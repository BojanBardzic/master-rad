//
// Created by bbard on 7/10/2024.
//

#include "ThemeManager.h"

std::unordered_map<ThemeName, Theme*> ThemeManager::m_themes = {
        {ThemeName::Light, new Theme(ThemeName::Light, ImColor(255, 255, 255), ImColor(0, 0, 0), ImColor(255, 166, 0), ImColor(230, 70, 37), ImColor(0, 0, 255),
                                     ImColor(224, 31, 212), ImColor(2, 165, 36), ImColor(0, 0, 0), ImColor(20, 20, 150, 180), ImColor(148, 116, 173), ImColor(150, 150, 150), ImColor(125, 125, 125))},
        {ThemeName::Dark, new Theme(ThemeName::Dark, ImColor(35, 35, 35), ImColor(255, 255, 255), ImColor(115, 227, 39), ImColor(245, 241, 6), ImColor(252, 121, 3),
                                    ImColor(235, 93, 215), ImColor(171, 179, 170), ImColor(255, 255, 255), ImColor(129, 129, 129, 180), ImColor(197, 199, 163), ImColor(150, 150, 150), ImColor(125, 125, 125))}
};

Theme* ThemeManager::m_theme = nullptr;


void ThemeManager::init() { m_theme = m_themes.at(m_defaultTheme); }

Theme *ThemeManager::getTheme() { return m_theme; }

void ThemeManager::setTheme(const ThemeName theme) { m_theme = m_themes.at(theme); }
