//
// Created by bbard on 6/30/2024.
//

#ifndef TEXT_EDITOR_FONT_H
#define TEXT_EDITOR_FONT_H

#include "FontManager.h"
#include "imgui.h"

#include <string>


class Font {
public:
    Font(const std::string& name, float size = FontManager::m_defaultSize);
    ~Font();

    void increaseSize();
    void decreaseSize();

    const std::string& getName() const;
    const float& getSize() const;
    ImFont* getFont() const;
private:
    void loadFont();

    std::string m_name;
    float m_size;
    ImFont* m_font;
};


#endif //TEXT_EDITOR_FONT_H
