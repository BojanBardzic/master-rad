//
// Created by bbard on 6/26/2024.
//

#ifndef TEXT_EDITOR_CURSOR_H
#define TEXT_EDITOR_CURSOR_H

#include "imgui.h"
#include <string>
#include <iostream>
#include <chrono>

class Cursor {
public:
    Cursor(size_t row = 1, size_t col = 1);
    ~Cursor();

    size_t getRow() const;
    size_t getCol() const;
    float getWidth() const;
    std::chrono::time_point<std::chrono::system_clock> getTimestamp() const;
    bool getShouldRender() const;

    void setWidth(float width);
    void setRow(size_t row);
    void setCol(size_t col);
    void setTimestamp(std::chrono::time_point<std::chrono::system_clock> timestamp);

    void calculateWidth();
    void updateShouldRender();

    ImVec2 getCursorPosition(ImVec2 cursorScreenPosition, std::string& line);
private:
    size_t m_row;
    size_t m_col;
    float m_width;
    std::chrono::time_point<std::chrono::system_clock> m_timestamp;
    const std::chrono::duration<double> m_drawInterval = std::chrono::duration<double>(0.75);
    bool m_shouldRender;
};


#endif //TEXT_EDITOR_CURSOR_H
