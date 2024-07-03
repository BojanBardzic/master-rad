//
// Created by bbard on 6/19/2024.
//

#include "TextBox.h"
#include <utility>

TextBox::TextBox(ImColor textColor, ImColor backgroundColor, float width, float height, std::string fontName)
    : m_textColor(textColor), m_backgroundColor(backgroundColor), m_width(width), m_height(height), m_xScroll(0.0f), m_yScroll(0) {

    FontManager::init();

    m_pieceTable = new PieceTable();
    m_cursor = new Cursor();
    m_font = new Font( fontName);

    // Fixme: Just a test input, will be removed later.
    m_pieceTable->insert("Hello World! World world world world world world world world world world world world\n\n\nHello There!\nSomething", 0);
    getLines();
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

    for (const std::string& line : m_lines) {
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
    size_t index = cursorPositionToBufferIndex();
    m_pieceTable->insert(std::move(str), index);
    getLines();
    moveCursorRight();
}

void TextBox::backspace() {
    size_t index = cursorPositionToBufferIndex();
    if (index != 0) {
        m_pieceTable->deleteText(index - 1, index);
        moveCursorLeft();
        getLines();
    }
}

void TextBox::deleteChar() {
    size_t index = cursorPositionToBufferIndex();
    if (index != m_pieceTable->getSize()) {
        m_pieceTable->deleteText(index, index+1);
        getLines();
    }
}

void TextBox::moveCursorRight() {

    auto row = m_cursor->getRow();
    auto col = m_cursor->getCol();

    if (m_lines.empty() || (col > m_lines[row-1].size() && row == m_lines.size())) {
        return;
    }

    m_cursor->resetTimer();

    if (col > m_lines[row-1].size()) {
        m_cursor->setCol(1);
        moveCursorDown();
    } else {
        m_cursor->setCol(col+1);
    }

    updateScroll();
}

void TextBox::moveCursorLeft() {

    auto row = m_cursor->getRow();
    auto col = m_cursor->getCol();

    if (m_lines.empty() || col == 1 && row == 1) {
        return;
    }

    m_cursor->resetTimer();

    if (col == 1) {
        m_cursor->setCol(m_lines[row-2].size() + 1);
        moveCursorUp();
    } else {
        m_cursor->setCol(col-1);
    }

    updateScroll();
}

void TextBox::moveCursorUp() {
    if (!m_lines.empty() && m_cursor->getRow() != 1) {
        m_cursor->resetTimer();
        m_cursor->setRow(m_cursor->getRow()-1);
        correctCursorColumn();
        updateScroll();
    }
}

void TextBox::moveCursorDown() {
    if (!m_lines.empty() && m_cursor->getRow() != m_lines.size()) {
        m_cursor->resetTimer();
        m_cursor->setRow(m_cursor->getRow()+1);
        correctCursorColumn();
        updateScroll();
    }
}

void TextBox::moveCursorToBeginning() {
    m_cursor->setCol(1);
    updateXScroll();
}

void TextBox::moveCursorToEnd() {
    if (!m_lines.empty()) {
        m_cursor->setCol(m_lines[m_cursor->getRow() - 1].size() + 1);
        updateXScroll();
    }
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

// Turns PieceTable data into lines.
void TextBox::getLines() {

    // Set up a string stream and load the PieceTable data into it
    std::stringstream stream;
    stream << *m_pieceTable;
    // Turn the string stream into a std::string
    std::string textBuffer = stream.str();

    // Clear the previous lines
    m_lines.clear();

    auto it = textBuffer.begin();

    while (it != textBuffer.end()) {
        // Find the newline in the buffer
        auto newLine = std::find(it, textBuffer.end(), '\n');
        // Adds the substring starting from the current iterator unitl the newline character
        m_lines.push_back(textBuffer.substr(it-textBuffer.begin(), newLine-it));

        // If we have reached the end of the buffer we break
        if (newLine == textBuffer.end())
            break;

        // If the last character in the buffer is a newLine add another line at the end
        if (std::next(newLine) == textBuffer.end())
            m_lines.push_back("");

        //We jump over the newline char and iterate to the next.
        it = std::next(newLine);
    }
}

void TextBox::drawCursor() {
    m_cursor->calculateWidth();

    ImGui::BeginChild("Child");
    ImGui::BeginChild("Cursor");

    std::string line = m_lines.empty() ? "" : m_lines[m_cursor->getRow()-1];

    auto cursorPosition = m_cursor->getCursorPosition(ImGui::GetCursorScreenPos(), line);
    cursorPosition.x -= m_xScroll;
    cursorPosition.y -= m_yScroll;

    ImGui::GetWindowDrawList()->AddRectFilled(cursorPosition, ImVec2(cursorPosition.x + m_cursor->getWidth(), cursorPosition.y + ImGui::GetFontSize()), ImColor(255, 0, 0));

    ImGui::EndChild();
    ImGui::EndChild();
}

void TextBox::correctCursorColumn() {
    if (m_cursor->getCol() > m_lines[m_cursor->getRow()-1].size() + 1) {
        moveCursorToEnd();
    }
}

size_t TextBox::cursorPositionToBufferIndex() {
    auto row = m_cursor->getRow();

    size_t index = 0;

    for (size_t i=0; i<row-1; i++) {
        index += m_lines[i].size() + 1;
    }

    index += (m_cursor->getCol() - 1);

    return index;
}

void TextBox::updateTextBoxSize() {
    m_width = ImGui::GetWindowWidth() - m_margin;
    m_height = ImGui::GetWindowHeight() - 2*m_margin;
}

// Calculates the x-axis advancement of the substring of the line at lineNumber
// between [begin, end)
float TextBox::getCursorXAdvance(size_t lineIndex, size_t begin, size_t end) {
    if (lineIndex >= m_lines.size() || begin >= m_lines[lineIndex].size() || end > m_lines[lineIndex].size() + 1 || begin > end) {
        return 0.0f;
    }

    float advance = 0.0f;
    auto line = m_lines[lineIndex];

    for (size_t i = begin; i<end; ++i) {
        advance += ImGui::GetFont()->GetCharAdvance(line[i]);
    }

    return advance;
}

void TextBox::updateXScroll() {
    ImGui::PushFont(m_font->getFont());

    auto cursorRow = m_cursor->getRow();
    auto cursorCol = m_cursor->getCol();

    auto advance = getCursorXAdvance(cursorRow - 1, 0, cursorCol);

    if (advance - m_xScroll > m_width) {
        m_xScroll = advance - m_width;
    } else if (advance != 0 && advance < m_xScroll) {
        m_xScroll = advance - ImGui::GetFont()->GetCharAdvance(m_lines[cursorRow-1][cursorCol-1]);
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



