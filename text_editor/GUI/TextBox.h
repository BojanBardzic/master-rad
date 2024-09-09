//
// Created by bbard on 6/19/2024.
//

#ifndef TEXT_EDITOR_TEXTBOX_H
#define TEXT_EDITOR_TEXTBOX_H

#include "imgui.h"

#include "Cursor.h"
#include "../File.h"
#include "Font.h"
#include "LineBuffer.h"
#include "MyRectangle.h"
#include "../PieceTable/PieceTableInstance.h"
#include "Selection.h"
#include "Scroll.h"
#include "../SyntaxHiglighting/TextHighlighter.h"
#include "Theme.h"
#include "ThemeManager.h"
#include "../CodeSnippets/SnippetManager.h"

#include <string>
#include <iostream>
#include <sstream>

class TextBox {
public:
    TextBox(float width, float height, const std::string& fontName, PieceTableInstance* instance = nullptr);
    ~TextBox();

    void draw();

    void enterChar(char c);
    void enterText(std::string str);
    void backspace();
    void tab(bool shift);
    void deleteChar();
    void deleteLine();
    void selectAll();
    void activateWriteSelection();
    void deactivateWriteSelection();
    void toggleRectangularSelection();
    void cut();
    void copy();
    void paste();
    void undo();
    void redo();

    void newFile();
    bool open(std::string& filePath);
    bool save();
    bool saveAs(std::string& filePath);
    bool saveSnippet(std::string& name);

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

    void getLines();
    bool isInsideTextBox(const ImVec2& point);
    bool isInsideHorizontalScrollbar(const ImVec2& point);
    bool isInsideVerticalScrollbar(const ImVec2& point);

    float getWidth() const;
    float getHeight() const;
    ImVec2 getTopLeft() const;
    ImVec2 getBottomRight() const;
    ImVec2 getTopLeftMargin() const;
    ImVec2 getBottomRightMargin() const;
    const MyRectangle& getHScrollbarRect() const;
    const MyRectangle& getVScrollbarRect() const;
    float getScrollbarSize() const;
    Cursor* getCursor() const;
    Theme* getTheme() const;
    File* getFile() const;
    PieceTableInstance* getPieceTableInstance() const;
    std::string getStatusBarText();
    bool isSelectionActive() const;
    bool isWriteSelectionActive() const;
    bool isRectangularSelectionActive() const;
    bool isDirty() const;
    bool isUndoEmpty() const;
    bool isRedoEmpty() const;

    void setWidth(float width);
    void setHeight(float height);
    void setTopLeftMargin(ImVec2 topLeftMargin);
    void setBottomRightMargin(ImVec2 bottomRightMargin);
    void setFile(File* file);
private:
    bool insertCharToPieceTable(char c);
    bool deleteSelection();
    void copySelectionToClipboard();
    TextCoordinates mousePositionToTextCoordinates(const ImVec2& mousePosition);

    bool addTabToSelectedRows();
    bool deleteTabFromSelectedRows();
    bool reverseTab();

    void drawRectangle(ImVec2 currentPosition, float& lineHeight);
    void drawText(ImVec2 textPosition, const std::string& line, size_t index);
    void drawSelection(Selection* selection, ImVec2 textPosition, std::string& line, size_t i, ThemeColor color);
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

    bool saveToFile();

    void updateUndoRedo();
    void updateTextBoxSize();
    void updateStateForTextChange(bool isInsert, size_t size);
    void updateWriteSelection(bool isInsert, size_t size);
    void updateStateForCursorMovement();
    void updateStateForSelectionChange();
private:
    PieceTableInstance* m_pieceTableInstance;
    LineBuffer* m_lineBuffer;
    File* m_file;
    Cursor* m_cursor;
    Selection* m_selection;
    Selection* m_writeSelection;
    Scroll* m_scroll;
    Font* m_font;
    bool m_dirty;
    float m_width;
    float m_height;
    ImVec2 m_topLeftMargin = {0.0f, 0.0f};
    ImVec2 m_bottomRightMargin = {5.0f, 5.0f};
    const float m_scrollbarSize = 15.0f;
    const float m_minScrollSelectSize = 20.0f;
};


#endif //TEXT_EDITOR_TEXTBOX_H
