//
// Created by bbard on 6/30/2024.
//

#include "FontManager.h"

const std::string FontManager::m_fontDirectoryPath = File::getProjectDirectory() + "GUI\\Fonts\\";
std::vector<std::string> FontManager::m_fontNames = {};
std::unordered_map<std::string, std::unordered_map<float, ImFont*>> FontManager::m_fonts = {};

ImFont* FontManager::getFont(std::string &name, float size) {
    return m_fonts[name][size];
}

void FontManager::init() {
    for (const auto& entry : std::filesystem::directory_iterator(m_fontDirectoryPath)) {
        if (entry.is_regular_file() && entry.path().extension().string() == ".ttf") {
            m_fontNames.push_back(entry.path().stem().string());
        }
    }

    for (auto fontName : m_fontNames) {
        std::string fontPath =  m_fontDirectoryPath + fontName + ".ttf";

        float size = m_minSize;
        while (size <= m_maxSize) {
            ImFont* font = ImGui::GetIO().Fonts->AddFontFromFileTTF(fontPath.c_str(), size);

            m_fonts[fontName].insert({size, font});

            size += m_offset;
        }
    }
}
