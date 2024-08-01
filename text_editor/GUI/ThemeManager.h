//
// Created by bbard on 7/10/2024.
//

#ifndef TEXT_EDITOR_THEMEMANAGER_H
#define TEXT_EDITOR_THEMEMANAGER_H

#include <unordered_map>
#include <string>
#include "Theme.h"

enum ThemeName {
    Light,
    Dark
};

class ThemeManager {
public:
    static Theme* getTheme(const ThemeName theme);
private:
    static std::unordered_map<ThemeName, Theme*> m_themes;
};


#endif //TEXT_EDITOR_THEMEMANAGER_H
