//
// Created by bbard on 7/10/2024.
//

#ifndef TEXT_EDITOR_THEMEMANAGER_H
#define TEXT_EDITOR_THEMEMANAGER_H

#include "Theme.h"

#include <string>
#include <unordered_map>

class ThemeManager {
public:
    static void init();
    static Theme* getTheme();
    static void setTheme(const ThemeName theme);
private:
    static Theme* m_theme;
    static const ThemeName m_defaultTheme = ThemeName::Light;
    static std::unordered_map<ThemeName, Theme*> m_themes;
};


#endif //TEXT_EDITOR_THEMEMANAGER_H
