//
// Created by bbard on 6/30/2024.
//

#ifndef TEXT_EDITOR_FONT_H
#define TEXT_EDITOR_FONT_H

#include <string>
#include "imgui.h"
#include "FontManager.h"

class Font {
public:
    Font(std::string& name, float size = FontManager::m_defaultSize);
    ~Font();

    void increaseSize();
    void decreaseSize();

    std::string getName() const;
    float getSize() const;
    ImFont* getFont() const;
private:
    void loadFont();

    std::string m_name;
    float m_size;
    ImFont* m_font;
};


#endif //TEXT_EDITOR_FONT_H
