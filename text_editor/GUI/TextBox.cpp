//
// Created by bbard on 6/19/2024.
//

#include "TextBox.h"
#include "../PieceTable/PieceTable.h"

TextBox::TextBox(ImColor textColor, ImColor backgroundColor, float width, float height)
    : m_textColor(textColor), m_backgroundColor(backgroundColor), m_width(width), m_height(height) {
    m_pieceTable = new PieceTable();
    m_cursor = new Cursor();

    m_pieceTable->insert("Hello World!\nHello There!\n", 0);
}

TextBox::~TextBox() {
    delete m_pieceTable;
    delete m_cursor;
}


void TextBox::draw() {
    bool* p_open = false;

    auto cursorScreenPosition = ImGui::GetCursorScreenPos();

    float lineHeight = ImGui::GetFontSize();

    auto lines = getLines();

    auto currentPosition = cursorScreenPosition;

    for (size_t i=0; i<lines.size(); ++i) {
        ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(currentPosition.x, currentPosition.y), ImVec2(currentPosition.x + m_width, currentPosition.y + lineHeight),
                                                  m_backgroundColor);
        ImGui::BeginChild("Child");
        ImGui::GetWindowDrawList()->AddText(ImVec2(currentPosition.x, currentPosition.y), m_textColor, lines[i].c_str());
        ImGui::EndChild();
        currentPosition.y = currentPosition.y + ImGui::GetFontSize();
    }

    float yOffset = currentPosition.y - cursorScreenPosition.y;

    if (yOffset < m_height) {
      ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(currentPosition.x, currentPosition.y), ImVec2(currentPosition.x + m_width, currentPosition.y + (m_height - yOffset)), m_backgroundColor);
    }


    m_cursor->draw(cursorScreenPosition, lines[m_cursor->getRow()-1]);
}

std::vector<std::string> TextBox::getLines() {
    std::stringstream stream;
    stream << *m_pieceTable;

    std::string textBuffer = stream.str();
    std::vector<std::string> lines;

    auto it = textBuffer.begin();

    while (it != textBuffer.end()) {
        auto newLine = std::find(it, textBuffer.end(), '\n');
        lines.push_back(textBuffer.substr(it-textBuffer.begin(), newLine-it));

        if (newLine == textBuffer.end())
            break;

        it = std::next(newLine);
    }

    return lines;
}

ImColor TextBox::getTextColor() const { return m_textColor; }

ImColor TextBox::getBackgroundColor() const { return m_backgroundColor; }

float TextBox::getWidth() const { return m_width; }

float TextBox::getHeight() const { return m_height; }

void TextBox::setTextColor(ImColor textColor) { m_textColor = textColor; }

void TextBox::setBackgroundColor(ImColor backgroundColor) { m_backgroundColor = backgroundColor; }

void TextBox::setWidth(float width) { m_width = width; }

void TextBox::setHeight(float height) { m_height = height; }
