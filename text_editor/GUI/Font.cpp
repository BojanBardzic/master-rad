//
// Created by bbard on 6/30/2024.
//

#include "Font.h"

Font::Font(std::string &name, float size) : m_name(name), m_size(size) {
    m_font = FontManager::getFont(m_name, m_size);
}

Font::~Font() {}

void Font::increaseSize() {
    if (m_size != FontManager::m_maxSize) {
        m_size = std::min(m_size + FontManager::m_offset, FontManager::m_maxSize);
        loadFont();
    }
}

void Font::decreaseSize() {
    if (m_size != FontManager::m_minSize) {
        m_size = std::max(m_size - FontManager::m_offset, FontManager::m_minSize);
        loadFont();
    }
}

std::string Font::getName() const { return m_name; }

float Font::getSize() const { return m_size; }

ImFont *Font::getFont() const { return m_font; }

void Font::loadFont() {
    m_font = FontManager::getFont(m_name, m_size);
}


