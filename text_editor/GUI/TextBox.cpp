//
// Created by bbard on 6/19/2024.
//

#include "TextBox.h"
#include <utility>

TextBox::TextBox(ImColor textColor, ImColor backgroundColor, float width, float height, std::string fontName)
    : m_textColor(textColor), m_backgroundColor(backgroundColor), m_width(width), m_height(height), m_xScroll(0.0f), m_yScroll(0) {

    FontManager::init();

    m_pieceTable = new PieceTable();
    m_lineBuffer = new LineBuffer(m_pieceTable);
    m_cursor = new Cursor(m_lineBuffer);
    m_font = new Font( fontName);

    // Fixme: Just a test input, will be removed later.
    m_pieceTable->insert("Hello World! World world world world world world world world world world world world\n\n\nHello There!\nSomething", 0);
    m_lineBuffer->getLines();
}

TextBox::~TextBox() {
    delete m_pieceTable;
    delete m_cursor;
    delete m_font;
}

// Draws the textBox based on PieceTable data.
void TextBox::draw() {

    auto cursorScreenPosition = ImGui::GetCursorScreenPos();

    // We track our current position as we draw the lines
    auto currentPosition = cursorScreenPosition;

    updateTextBoxSize();

    // Apply the current font
    ImGui::PushFont(m_font->getFont());

    float lineHeight = ImGui::GetFontSize();

    auto linesSize = m_lineBuffer->getSize();

    for (size_t i=0; i<linesSize; ++i) {
        // Get the line
        auto line = m_lineBuffer->lineAt(i);

        // Add a clip rectangle
        ImGui::GetWindowDrawList()->PushClipRect(ImVec2(currentPosition.x, currentPosition.y), ImVec2(currentPosition.x + m_width, currentPosition.y + lineHeight));
        // Draw the background rectangle
        ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(currentPosition.x, currentPosition.y), ImVec2(currentPosition.x + m_width, currentPosition.y + lineHeight),
                                                  m_backgroundColor);
        // Deactivate clip rectangle
        ImGui::GetWindowDrawList()->PopClipRect();

        // We use ImGui::BeginChild to state that the text should be rendered in front of the background
        ImGui::BeginChild("Child");
        // Add a clip rectangle
        ImGui::GetWindowDrawList()->PushClipRect(ImVec2(cursorScreenPosition.x, cursorScreenPosition.y), ImVec2(currentPosition.x + m_width, currentPosition.y + lineHeight + 2.0f));
        // Draw the line text
        ImGui::GetWindowDrawList()->AddText(ImVec2(currentPosition.x - m_xScroll, currentPosition.y - m_yScroll), m_textColor, line.c_str());
        // Deactivate clip rectangle
        ImGui::GetWindowDrawList()->PopClipRect();
        ImGui::EndChild();

        // Update the current position
        currentPosition.y = currentPosition.y + ImGui::GetFontSize();
    }

    float yOffset = currentPosition.y - cursorScreenPosition.y;

    if (yOffset < m_height) {
      ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(currentPosition.x, currentPosition.y), ImVec2(currentPosition.x + m_width, currentPosition.y + (m_height - yOffset)), m_backgroundColor);
    }

    if (m_cursor->getShouldRender())
        drawCursor();
    m_cursor->updateShouldRender();

    ImGui::PopFont();
}

// Enters the text in the buffer
void TextBox::enterChar(std::string str) {
    std::cerr << str << std::endl;
    size_t index = m_cursor->positionToBufferIndex();
    m_pieceTable->insert(std::move(str), index);
    m_lineBuffer->getLines();
    moveCursorRight();
}

void TextBox::backspace() {
    size_t index = m_cursor->positionToBufferIndex();
    if (index != 0) {
        m_pieceTable->deleteText(index - 1, index);
        moveCursorLeft();
        m_lineBuffer->getLines();
    }
}

void TextBox::deleteChar() {
    size_t index = m_cursor->positionToBufferIndex();
    if (index != m_pieceTable->getSize()) {
        m_pieceTable->deleteText(index, index+1);
        m_lineBuffer->getLines();
    }
}

void TextBox::moveCursorRight() {
    m_cursor->moveRight();
    updateScroll();
}

void TextBox::moveCursorLeft() {
    m_cursor->moveLeft();
    updateScroll();
}

void TextBox::moveCursorUp() {
    m_cursor->moveUp();
    updateScroll();
}

void TextBox::moveCursorDown() {
    m_cursor->moveDown();
    updateScroll();
}

void TextBox::moveCursorToBeginning() {
    m_cursor->moveToBeginning();
    updateXScroll();
}

void TextBox::moveCursorToEnd() {
    m_cursor->moveToEnd();
    updateXScroll();
}

void TextBox::increaseFontSize() {
    m_font->increaseSize();
}

void TextBox::decreaseFontSize() {
    m_font->decreaseSize();
}

ImColor TextBox::getTextColor() const { return m_textColor; }

ImColor TextBox::getBackgroundColor() const { return m_backgroundColor; }

float TextBox::getWidth() const { return m_width; }

float TextBox::getHeight() const { return m_height; }

Cursor *TextBox::getCursor() const { return m_cursor; }

void TextBox::setTextColor(ImColor textColor) { m_textColor = textColor; }

void TextBox::setBackgroundColor(ImColor backgroundColor) { m_backgroundColor = backgroundColor; }

void TextBox::setWidth(float width) { m_width = width; }

void TextBox::setHeight(float height) { m_height = height; }

void TextBox::drawCursor() {
    m_cursor->calculateWidth();

    ImGui::BeginChild("Child");
    ImGui::BeginChild("Cursor");

    std::string line = m_lineBuffer->isEmpty() ? "" : m_lineBuffer->lineAt(m_cursor->getRow()-1);

    auto cursorPosition = m_cursor->getCursorPosition(ImGui::GetCursorScreenPos());
    cursorPosition.x -= m_xScroll;
    cursorPosition.y -= m_yScroll;

    ImGui::GetWindowDrawList()->AddRectFilled(cursorPosition, ImVec2(cursorPosition.x + m_cursor->getWidth(), cursorPosition.y + ImGui::GetFontSize()), ImColor(255, 0, 0));

    ImGui::EndChild();
    ImGui::EndChild();
}


void TextBox::updateTextBoxSize() {
    m_width = ImGui::GetWindowWidth() - m_margin;
    m_height = ImGui::GetWindowHeight() - 2*m_margin;
}

void TextBox::updateXScroll() {
    ImGui::PushFont(m_font->getFont());

    auto cursorRow = m_cursor->getRow();
    auto cursorCol = m_cursor->getCol();

    auto advance = m_cursor->getXAdvance();

    if (advance - m_xScroll > m_width) {
        m_xScroll = advance - m_width;
    } else if (advance != 0 && advance < m_xScroll) {
        m_xScroll = advance - ImGui::GetFont()->GetCharAdvance(m_lineBuffer->lineAt(cursorRow-1)[cursorCol-1]);
    } else if (advance == 0) {
        m_xScroll = 0;
    }

    ImGui::PopFont();
}

void TextBox::updateYScroll() {
    ImGui::PushFont(m_font->getFont());

    auto cursorRow = m_cursor->getRow();
    auto lineHeight = ImGui::GetFontSize();

    std::cerr << "ROW: " << cursorRow << std::endl;

    if (cursorRow * lineHeight - m_yScroll > m_height) {
        m_yScroll = cursorRow * lineHeight - m_height;
    } else if (cursorRow != 1 && m_yScroll >= (cursorRow * lineHeight)) {
        m_yScroll = (cursorRow-1) * lineHeight;
    } else if (cursorRow == 1) {
        m_yScroll = 0;
    }

    ImGui::PopFont();
}

inline void TextBox::updateScroll() {
    updateXScroll();
    updateYScroll();
}



