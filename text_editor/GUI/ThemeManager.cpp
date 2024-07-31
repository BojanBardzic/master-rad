//
// Created by bbard on 7/10/2024.
//

#include "ThemeManager.h"

std::unordered_map<std::string, Theme*> ThemeManager::m_themes = {
        {"Light", new Theme("Light", ImColor(255, 255, 255), ImColor(0, 0, 0), ImColor(255, 166, 0), ImColor(230, 70, 37), ImColor(0, 0, 255),
                            ImColor(0, 0, 0), ImColor(20, 20, 150, 180), ImColor(150, 150, 150), ImColor(125, 125, 125))},
        {"Dark", new Theme("Dark", ImColor(35, 35, 35), ImColor(255, 255, 255), ImColor(115, 227, 39), ImColor(245, 241, 6), ImColor(235, 93, 215),
                           ImColor(255, 255, 255), ImColor(129, 129, 129, 180), ImColor(150, 150, 150), ImColor(125, 125, 125))}
};


Theme *ThemeManager::getTheme(std::string name) {
    return m_themes[name];
}
