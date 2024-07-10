//
// Created by bbard on 7/10/2024.
//

#ifndef TEXT_EDITOR_THEMEMANAGER_H
#define TEXT_EDITOR_THEMEMANAGER_H

#include <unordered_map>
#include <string>
#include "Theme.h"

class ThemeManager {
public:
    static Theme* getTheme(std::string name);
private:
    static std::unordered_map<std::string, Theme*> m_themes;
};


#endif //TEXT_EDITOR_THEMEMANAGER_H
