//
// Created by bbard on 6/19/2024.
//

#include "TextBox.h"

TextBox::TextBox(ImColor textColor, ImColor backgroundColor, float width, float height)
    : m_textColor(textColor), m_backgroundColor(backgroundColor), m_width(width), m_height(height) {
    m_pieceTable = new PieceTable();
    m_cursor = new Cursor();

    m_pieceTable->insert("Hello World!\nHello There!\nSomething", 0);
}

TextBox::~TextBox() {
    delete m_pieceTable;
    delete m_cursor;
}


void TextBox::draw() {

    auto cursorScreenPosition = ImGui::GetCursorScreenPos();

    float lineHeight = ImGui::GetFontSize();

    getLines();

    auto currentPosition = cursorScreenPosition;

    for (const std::string& line : m_lines) {
        ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(currentPosition.x, currentPosition.y), ImVec2(currentPosition.x + m_width, currentPosition.y + lineHeight),
                                                  m_backgroundColor);
        ImGui::BeginChild("Child");
        ImGui::GetWindowDrawList()->AddText(ImVec2(currentPosition.x, currentPosition.y), m_textColor, line.c_str());
        ImGui::EndChild();
        currentPosition.y = currentPosition.y + ImGui::GetFontSize();
    }

    float yOffset = currentPosition.y - cursorScreenPosition.y;

    if (yOffset < m_height) {
      ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(currentPosition.x, currentPosition.y), ImVec2(currentPosition.x + m_width, currentPosition.y + (m_height - yOffset)), m_backgroundColor);
    }


    m_cursor->draw(cursorScreenPosition, m_lines[m_cursor->getRow()-1]);
}

void TextBox::getLines() {
    std::stringstream stream;
    stream << *m_pieceTable;

    std::string textBuffer = stream.str();
    m_lines.clear();

    auto it = textBuffer.begin();

    while (it != textBuffer.end()) {
        auto newLine = std::find(it, textBuffer.end(), '\n');
        m_lines.push_back(textBuffer.substr(it-textBuffer.begin(), newLine-it));

        if (newLine == textBuffer.end())
            break;

        it = std::next(newLine);
    }
}

void TextBox::moveCursorRight() {
    auto row = m_cursor->getRow();
    auto col = m_cursor->getCol();

    if (col > m_lines[row-1].size() && row == m_lines.size()) {
        return;
    }

    if (col > m_lines[row-1].size()) {
        m_cursor->setCol(1);
        moveCursorDown();
    } else {
        m_cursor->setCol(col+1);
    }
}

void TextBox::moveCursorLeft() {
    auto row = m_cursor->getRow();
    auto col = m_cursor->getCol();

    if (col == 1 && row == 1) {
        return;
    }

    if (col == 1) {
        m_cursor->setCol(m_lines[row-2].size() + 1);
        moveCursorUp();
    } else {
        m_cursor->setCol(col-1);
    }
}

void TextBox::moveCursorUp() {
    if (m_cursor->getRow() != 1) {
        m_cursor->setRow(m_cursor->getRow()-1);
        correctCursorColumn();
    }
}

void TextBox::moveCursorDown() {
    if (m_cursor->getRow() != m_lines.size()) {
        m_cursor->setRow(m_cursor->getRow()+1);
        correctCursorColumn();
    }
}

void TextBox::correctCursorColumn() {
    if (m_cursor->getCol() > m_lines[m_cursor->getRow()-1].size() + 1) {
        moveCursorToEnd();
    }
}

void TextBox::moveCursorToBegining() {
    m_cursor->setCol(1);
}

void TextBox::moveCursorToEnd() {
    m_cursor->setCol(m_lines[m_cursor->getRow()-1].size() + 1);
}

ImColor TextBox::getTextColor() const { return m_textColor; }

ImColor TextBox::getBackgroundColor() const { return m_backgroundColor; }

float TextBox::getWidth() const { return m_width; }

float TextBox::getHeight() const { return m_height; }

void TextBox::setTextColor(ImColor textColor) { m_textColor = textColor; }

void TextBox::setBackgroundColor(ImColor backgroundColor) { m_backgroundColor = backgroundColor; }

void TextBox::setWidth(float width) { m_width = width; }

void TextBox::setHeight(float height) { m_height = height; }

Cursor *TextBox::getCursor() const { return m_cursor; }



