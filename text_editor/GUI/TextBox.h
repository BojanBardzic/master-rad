//
// Created by bbard on 6/19/2024.
//

#ifndef TEXT_EDITOR_TEXTBOX_H
#define TEXT_EDITOR_TEXTBOX_H

#include "imgui.h"
#include "../PieceTable/PieceTable.h"
#include "LineBuffer.h"
#include "Cursor.h"
#include "Font.h"
#include "Selection.h"
#include <string>
#include <iostream>
#include <sstream>

class TextBox {
public:
    TextBox(ImColor textColor = ImColor(0, 0, 0), ImColor backgroundColor = ImColor(255, 255, 255),
            float width = 200.0f, float height = 400.0f, std::string fontName = "Consolas");
    ~TextBox();

    void draw();

    void enterChar(std::string str);
    void backspace();
    void deleteChar();

    void selectAll();

    void moveCursorRight(bool shift);
    void moveCursorLeft(bool shift);
    void moveCursorUp(bool shift);
    void moveCursorDown(bool shift);
    void moveCursorToBeginning(bool shift);
    void moveCursorToEnd(bool shift);
    void moveCursorToMousePosition(ImVec2& mousePosition);
    void setMouseSelection(ImVec2& endPosition, ImVec2& delta);

    void increaseFontSize();
    void decreaseFontSize();

    ImColor getTextColor() const;
    ImColor getBackgroundColor() const;
    float getWidth() const;
    float getHeight() const;
    Cursor* getCursor() const;

    void setBackgroundColor(ImColor backgroundColor);
    void setTextColor(ImColor textColor);
    void setWidth(float width);
    void setHeight(float height);
private:
    bool deleteSelection();
    TextCoordinates mousePositionToTextCoordinates(const ImVec2& mousePosition);

    void drawRectangle(ImVec2 currentPosition, float lineHeight);
    void drawCursor();
    void updateTextBoxSize();
    static float getXAdvance(std::string& str);

    void updateXScroll();
    void updateYScroll();
    void updateScroll();

private:
    PieceTable* m_pieceTable;
    LineBuffer* m_lineBuffer;
    Cursor* m_cursor;
    Selection* m_selection;
    float m_width;
    float m_height;
    float m_xScroll;
    float m_yScroll;
    const float m_margin = 20.0f;
    ImColor m_backgroundColor;
    ImColor m_textColor;
    Font* m_font;
};


#endif //TEXT_EDITOR_TEXTBOX_H
