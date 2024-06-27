//
// Created by bbard on 6/19/2024.
//

#ifndef TEXT_EDITOR_TEXTBOX_H
#define TEXT_EDITOR_TEXTBOX_H

#include "imgui.h"
#include "../PieceTable/PieceTable.h"
#include "Cursor.h"
#include <string>
#include <iostream>
#include <sstream>

class TextBox {
public:
    TextBox(ImColor textColor = ImColor(0, 0, 0), ImColor backgroundColor = ImColor(255, 255, 255),
            float width = 200.0f, float height = 400.0f);
    ~TextBox();

    ImColor getTextColor() const;
    ImColor getBackgroundColor() const;
    float getWidth() const;
    float getHeight() const;

    void setBackgroundColor(ImColor backgroundColor);
    void setTextColor(ImColor textColor);
    void setWidth(float width);
    void setHeight(float height);

    void draw();
private:
    std::vector<std::string> getLines();

private:
    PieceTable* m_pieceTable;
    Cursor* m_cursor;
    float m_width;
    float m_height;
    ImColor m_backgroundColor;
    ImColor m_textColor;
};


#endif //TEXT_EDITOR_TEXTBOX_H