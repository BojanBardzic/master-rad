//
// Created by bbard on 6/30/2024.
//

#ifndef TEXT_EDITOR_FONTMANAGER_H
#define TEXT_EDITOR_FONTMANAGER_H

#include "imgui.h"
#include <string>
#include <iostream>
#include <unordered_map>

class FontManager {
public:
    static ImFont* getFont(std::string& name, float size);
    static void init();
public:
    constexpr static float m_maxSize = 61.0f;
    constexpr static float m_minSize =  9.0f;
    constexpr static float m_defaultSize = 13.0f;
    constexpr static float m_offset = 2.0f;
private:
    static std::unordered_map<std::string, std::string> m_fontPaths;
    static std::unordered_map<std::string, std::unordered_map<float, ImFont*>> m_fonts;
};


#endif //TEXT_EDITOR_FONTMANAGER_H
