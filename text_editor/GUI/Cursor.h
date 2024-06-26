//
// Created by bbard on 6/26/2024.
//

#ifndef TEXT_EDITOR_CURSOR_H
#define TEXT_EDITOR_CURSOR_H

#include "imgui.h"
#include <string>
#include <iostream>

class Cursor {
public:
    Cursor(size_t row = 1, size_t col = 1);
    ~Cursor();

    void draw(ImVec2 cursorScreenPosition, std::string& line);

    size_t getRow() const;
    size_t getCol() const;
    float getWidth() const;

    void setWidth(float width);
    void setRow(size_t row);
    void setCol(size_t col);

    ImVec2 getCursorPosition(ImVec2 cursorScreenPosition, std::string& line);
private:
    void calculateWidth();

    float m_width;
    size_t m_row;
    size_t m_col;
};


#endif //TEXT_EDITOR_CURSOR_H
