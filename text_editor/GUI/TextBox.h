//
// Created by bbard on 6/19/2024.
//

#ifndef TEXT_EDITOR_TEXTBOX_H
#define TEXT_EDITOR_TEXTBOX_H

#include "imgui.h"

#include "Cursor.h"
#include "Font.h"
#include "LineBuffer.h"
#include "MyRectangle.h"
#include "../PieceTable/PieceTable.h"
#include "../PieceTable/PieceTableInstance.h"
#include "Selection.h"
#include "Scroll.h"
#include "Theme.h"

#include <string>
#include <iostream>
#include <sstream>

class TextBox {
public:
    TextBox(float width = 200.0f, float height = 400.0f,
            std::string fontName = "Consolas", std::string theme = "Light");
    ~TextBox();

    void draw();

    void enterChar(char c);
    void enterText(std::string str);
    void backspace();
    void deleteChar();
    void selectAll();
    void cut();
    void copy();
    void paste();
    void undo();
    void redo();

    void newFile();
    bool open(std::string& filename);

    void moveCursorRight(bool shift);
    void moveCursorLeft(bool shift);
    void moveCursorUp(bool shift);
    void moveCursorDown(bool shift);
    void moveCursorToBeginning(bool shift);
    void moveCursorToEnd(bool shift);

    void moveCursorToMousePosition(ImVec2& mousePosition);
    void setMouseSelection(ImVec2& endPosition, ImVec2& delta);
    void mouseWheelScroll(bool shift, float& mouseWheel);
    void horizontalScroll(ImVec2& mousePosition, ImVec2& delta);
    void verticalScroll(ImVec2& mousePosition, ImVec2& delta);
    void horizontalBarClick(ImVec2& mousePosition);
    void verticalBarClick(ImVec2& mousePosition);

    void increaseFontSize();
    void decreaseFontSize();

    float getWidth() const;
    float getHeight() const;
    ImVec2 getTextBoxPosition() const;
    const MyRectangle& getHScrollbarRect() const;
    const MyRectangle& getVScrollbarRect() const;
    float getScrollbarSize() const;
    Cursor* getCursor() const;
    bool isSelectionActive() const;

    void setWidth(float width);
    void setHeight(float height);
private:
    bool deleteSelection();
    void copySelectionToClipboard();
    TextCoordinates mousePositionToTextCoordinates(const ImVec2& mousePosition);

    void drawRectangle(ImVec2 currentPosition, float& lineHeight);
    void drawCursor();
    void drawScrollBars();
    void drawHorizontalScrollbar();
    void drawVerticalScrollbar();
    void drawHScrollbarRect();
    void drawVScrollbarRect();
    void updateHScrollbarRect();
    void updateVScrollbarRect();
    void updateHScrollSelectRect();
    void updateVScrollSelectRect();
    void drawRect(const MyRectangle& rect, const ImColor& color);

    void updateUndoRedo();
    void updateTextBoxSize();
private:
    PieceTableInstance* m_pieceTableInstance;
    LineBuffer* m_lineBuffer;
    Cursor* m_cursor;
    Selection* m_selection;
    Scroll* m_scroll;
    Font* m_font;
    Theme* m_theme;
    bool m_dirty;
    float m_width;
    float m_height;
    const float m_scrollbarSize = 15.0f;
    const float m_minScrollSelectSize = 20.0f;
    const ImVec2 m_topLeftMargin = {0.0f, 0.0f};
    const ImVec2 m_bottomRightMargin = {30.0f, 30.0f};
};


#endif //TEXT_EDITOR_TEXTBOX_H
