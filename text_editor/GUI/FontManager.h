//
// Created by bbard on 6/30/2024.
//

#ifndef TEXT_EDITOR_FONTMANAGER_H
#define TEXT_EDITOR_FONTMANAGER_H

#include "imgui.h"
#include "../File.h"

#include <filesystem>
#include <iostream>
#include <string>
#include <unordered_map>

class FontManager {
public:
    static ImFont* getFont(std::string& name, float size);
    static void init();
public:
    constexpr static float m_maxSize = 61.0f;
    constexpr static float m_minSize =  9.0f;
    constexpr static float m_defaultSize = 15.0f;
    constexpr static float m_offset = 2.0f;
private:
    static const std::string m_fontDirectoryPath;
    static std::vector<std::string> m_fontNames;
    static std::unordered_map<std::string, std::unordered_map<float, ImFont*>> m_fonts;
};


#endif //TEXT_EDITOR_FONTMANAGER_H
