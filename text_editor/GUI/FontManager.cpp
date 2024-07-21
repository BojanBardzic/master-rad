//
// Created by bbard on 6/30/2024.
//

#include "FontManager.h"

std::unordered_map<std::string, std::string> FontManager::m_fontPaths = {};
std::unordered_map<std::string, std::unordered_map<float, ImFont*>> FontManager::m_fonts = {};

ImFont* FontManager::getFont(std::string &name, float size) {
    return m_fonts[name][size];
}

void FontManager::init() {
    FontManager::m_fontPaths.insert({"Consolas", "C:\\Users\\bbard\\OneDrive\\Desktop\\MasterRad\\master-rad\\text_editor\\GUI\\Fonts\\Consolas.ttf"});
    FontManager::m_fontPaths.insert({"Segoe UI", "C:\\Users\\bbard\\OneDrive\\Desktop\\MasterRad\\master-rad\\text_editor\\GUI\\Fonts\\Segoe UI.ttf"});

    for (auto entry : m_fontPaths) {
        auto fontName = entry.first;
        auto fontPath = entry.second;

        float size = m_minSize;
        while (size <= m_maxSize) {
            ImFont* font = ImGui::GetIO().Fonts->AddFontFromFileTTF(fontPath.c_str(), size);

            m_fonts[fontName].insert({size, font});

            size += m_offset;
        }
    }
}
