//
// Created by bbard on 6/26/2024.
//

#ifndef TEXT_EDITOR_CURSOR_H
#define TEXT_EDITOR_CURSOR_H

#include "imgui.h"

#include <chrono>
#include <iostream>
#include <numeric>
#include <string>
#include <stack>

#include "LineBuffer.h"
#include "TextPosition.h"

class Cursor {
public:
    Cursor(LineBuffer* lineBuffer, size_t row = 1, size_t col = 1);
    ~Cursor();

    void moveRight(size_t times = 1);
    void moveLeft(size_t times = 1);
    void moveUp(size_t times = 1);
    void moveDown(size_t times = 1);
    void moveToBeginning();
    void moveToEnd();
    void moveToEndOfFile();

    void recordCursorPosition();
    void cursorUndo();
    void cursorRedo();
    void clearUndoAndRedoStacks();

    const float getXAdvance() const;

    const size_t& getRow() const;
    const size_t& getCol() const;
    const TextCoordinates& getCoords() const;
    const float& getWidth() const;
    bool getShouldRender() const;

    void setWidth(const float& width);
    void setRow(const size_t& row);
    void setCol(const size_t& col);
    void setCoords(const TextCoordinates& coords);

    void clipCursor(const TextCoordinates& start, const TextCoordinates& end);

    void calculateWidth();
    void updateShouldRender();

    ImVec2 getCursorPosition(const ImVec2& cursorScreenPosition);
    float getXAdvance(const std::string& str);
private:
    void resetTimer();

    void clearUndoStack();
    void clearRedoStack();

    TextPosition m_position;
    LineBuffer* m_lineBuffer;
    float m_width;
    std::stack<TextCoordinates> m_cursorUndoStack;
    std::stack<TextCoordinates> m_cursorRedoStack;
    std::chrono::time_point<std::chrono::system_clock> m_timestamp;
    const std::chrono::duration<double> m_drawInterval = std::chrono::duration<double>(0.75);
    bool m_shouldRender;
};


#endif //TEXT_EDITOR_CURSOR_H
