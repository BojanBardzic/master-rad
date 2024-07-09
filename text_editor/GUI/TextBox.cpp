//
// Created by bbard on 6/19/2024.
//

#include "TextBox.h"
#include <utility>

TextBox::TextBox(ImColor textColor, ImColor backgroundColor, float width, float height, std::string fontName)
    : m_textColor(textColor), m_backgroundColor(backgroundColor), m_width(width), m_height(height) {

    FontManager::init();

    m_pieceTable = new PieceTable();
    m_lineBuffer = new LineBuffer(m_pieceTable);
    m_cursor = new Cursor(m_lineBuffer);
    m_selection = new Selection(m_lineBuffer);
    m_font = new Font(fontName);
    m_scroll = new Scroll(m_lineBuffer, m_cursor, m_font);

    // Fixme: Just a test input, will be removed later.
    m_pieceTable->insert("Hello World! World world world world world world world world world world world world\n\n\nHello There!\nSomething", 0);
    m_lineBuffer->getLines();
}

TextBox::~TextBox() {
    delete m_scroll;
    delete m_font;
    delete m_selection;
    delete m_cursor;
    delete m_lineBuffer;
    delete m_pieceTable;
}

// Draws the textBox based on PieceTable data.
void TextBox::draw() {
    if(!m_scroll->isInit())
        m_scroll->init(m_width, m_height);

    auto cursorScreenPosition = ImGui::GetCursorScreenPos();

    // We track our current position as we draw the lines
    auto currentPosition = cursorScreenPosition;

    updateTextBoxSize();

    // Apply the current font
    ImGui::PushFont(m_font->getFont());

    auto lineHeight = ImGui::GetFontSize();
    auto linesSize = m_lineBuffer->getSize();

    for (size_t i=0; i<linesSize; ++i) {
        // Get the line
        auto line = m_lineBuffer->lineAt(i);

        // Draw background rectangle
        drawRectangle(currentPosition, lineHeight);

        // Add a clip rectangle
        ImGui::GetWindowDrawList()->PushClipRect(ImVec2(cursorScreenPosition.x, cursorScreenPosition.y), ImVec2(currentPosition.x + m_width, currentPosition.y + lineHeight + 2.0f));

        auto overlap = m_selection->getIntersectionWithLine(i);

        auto xScroll = m_scroll->getXScroll();
        auto yScroll = m_scroll->getYScroll();

        if (m_selection->getActive() && overlap != std::pair{0, 0}) {
            auto leftOverlap = overlap.first;
            auto rightOverlap = overlap.second;

            auto leftString = line.substr(0, leftOverlap);
            auto middleString = line.substr(leftOverlap, rightOverlap-leftOverlap);
            auto rightString = line.substr(rightOverlap);

            float xAdvance = 0.0f;

            if (!leftString.empty()) {
                ImGui::GetWindowDrawList()->AddText(ImVec2(currentPosition.x - xScroll, currentPosition.y - yScroll), m_textColor, leftString.c_str());
                xAdvance += m_cursor->getXAdvance(leftString);
            }

            auto secondStringAdvance = m_cursor->getXAdvance(middleString);
            ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(currentPosition.x + xAdvance - xScroll, currentPosition.y - yScroll), ImVec2(currentPosition.x + xAdvance + secondStringAdvance, currentPosition.y + lineHeight), ImColor(20, 20, 150));
            ImGui::GetWindowDrawList()->AddText(ImVec2(currentPosition.x + xAdvance - xScroll, currentPosition.y - yScroll), ImColor(255, 255, 255), middleString.c_str());
            xAdvance += secondStringAdvance;


            if (!rightString.empty()) {
                ImGui::GetWindowDrawList()->AddText(ImVec2(currentPosition.x + xAdvance - xScroll, currentPosition.y - yScroll), m_textColor, rightString.c_str());
            }

        } else {
            // Draw the line text
            ImGui::GetWindowDrawList()->AddText(ImVec2(currentPosition.x - xScroll, currentPosition.y - yScroll), m_textColor, line.c_str());
        }

        // Deactivate clip rectangle
        ImGui::GetWindowDrawList()->PopClipRect();

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
    deleteSelection();

    auto coords = m_cursor->getCoords();
    size_t index = m_lineBuffer->textCoordinatesToBufferIndex(coords);
    m_pieceTable->insert(std::move(str), index);
    m_lineBuffer->getLines();
    m_cursor->moveRight();
    m_scroll->updateScroll(m_width, m_height);
    m_scroll->updateMaxScroll(m_width, m_height);
}

void TextBox::backspace() {
    auto deleted = deleteSelection();

    if (deleted) {
        return;
    }

    auto coords = m_cursor->getCoords();
    size_t index = m_lineBuffer->textCoordinatesToBufferIndex(coords);
    if (index != 0) {
        m_pieceTable->deleteText(index - 1, index);
        m_cursor->moveLeft();
        m_scroll->updateScroll(m_width, m_height);
        m_lineBuffer->getLines();
        m_scroll->updateMaxScroll(m_width, m_height);
    }
}

void TextBox::deleteChar() {
    auto deleted = deleteSelection();

    if (deleted)
        return;

    auto coords = m_cursor->getCoords();
    size_t index = m_lineBuffer->textCoordinatesToBufferIndex(coords);
    if (index != m_pieceTable->getSize()) {
        m_pieceTable->deleteText(index, index+1);
        m_lineBuffer->getLines();
        m_scroll->updateMaxScroll(m_width, m_height);
    }
}

void TextBox::selectAll() {
    m_cursor->moveToEndOfFile();
    auto newCoords = m_cursor->getCoords();
    m_selection->selectAll(newCoords);
}

void TextBox::moveCursorRight(bool shift) {

    auto oldCoords = m_cursor->getCoords();

    m_cursor->moveRight();
    m_scroll->updateScroll(m_width, m_height);

    auto newCoords = m_cursor->getCoords();

    if (shift) {
        m_selection->update(oldCoords, newCoords);
    } else {
        if (m_selection->getActive()) {
            m_cursor->setCoords(m_selection->getEnd());
        }
        m_selection->setActive(false);
    }
}

void TextBox::moveCursorLeft(bool shift) {
    auto oldCoords = m_cursor->getCoords();

    m_cursor->moveLeft();
    m_scroll->updateScroll(m_width, m_height);

    auto newCoords = m_cursor->getCoords();

    if (shift) {
        m_selection->update(oldCoords, newCoords);
    } else {
        if (m_selection->getActive()) {
            m_cursor->setCoords(m_selection->getStart());
        }
        m_selection->setActive(false);
    }
}

void TextBox::moveCursorUp(bool shift) {
    auto oldCoords = m_cursor->getCoords();

    m_cursor->moveUp();
    m_scroll->updateScroll(m_width, m_height);

    auto newCoords = m_cursor->getCoords();

    if (shift) {
        m_selection->update(oldCoords, newCoords);
    } else {
        m_selection->setActive(false);
    }
}

void TextBox::moveCursorDown(bool shift) {
    auto oldCoords = m_cursor->getCoords();

    m_cursor->moveDown();
    m_scroll->updateScroll(m_width, m_height);

    auto newCoords = m_cursor->getCoords();

    if (shift) {
        m_selection->update(oldCoords, newCoords);
    } else {
        m_selection->setActive(false);
    }
}

void TextBox::moveCursorToBeginning(bool shift) {
    auto oldCoords = m_cursor->getCoords();

    m_cursor->moveToBeginning();
    m_scroll->updateXScroll(m_width);

    auto newCoords = m_cursor->getCoords();

    if (shift) {
        m_selection->update(oldCoords, newCoords);
    } else {
        m_selection->setActive(false);
    }
}

void TextBox::moveCursorToEnd(bool shift) {
    auto oldCoords = m_cursor->getCoords();

    m_cursor->moveToEnd();
    m_scroll->updateXScroll(m_width);

    auto newCoords = m_cursor->getCoords();

    if (shift) {
        m_selection->update(oldCoords, newCoords);
    } else {
        m_selection->setActive(false);
    }
}

void TextBox::moveCursorToMousePosition(ImVec2& mousePosition) {
    m_selection->setActive(false);
    auto coords = mousePositionToTextCoordinates(mousePosition);
    m_cursor->setCoords(coords);
}

void TextBox::setMouseSelection(ImVec2& endPosition, ImVec2& delta) {
    const ImVec2 startPosition = {endPosition.x - delta.x, endPosition.y - delta.y};
    auto startPositionCoords = mousePositionToTextCoordinates(startPosition);
    auto endPositionCoords = mousePositionToTextCoordinates(endPosition);

    if (startPositionCoords > endPositionCoords) {
        auto tmp = startPositionCoords;
        startPositionCoords = endPositionCoords;
        endPositionCoords = tmp;
    }

    m_selection->setStart(startPositionCoords);
    m_selection->setEnd(endPositionCoords);
    m_selection->setActive(true);
    m_cursor->setCoords(endPositionCoords);
}

void TextBox::mouseWheelScroll(bool shift, float &mouseWheel) {
    if (shift)
        m_scroll->mouseWheelXScroll(mouseWheel);
    else
        m_scroll->mouseWheelYScroll(mouseWheel);
}

void TextBox::increaseFontSize() {
    m_font->increaseSize();
    m_scroll->updateMaxScroll(m_width, m_height);
}

void TextBox::decreaseFontSize() {
    m_font->decreaseSize();
    m_scroll->updateMaxScroll(m_width, m_height);
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

inline void TextBox::drawRectangle(ImVec2 currentPosition, float lineHeight) {
    // Add a clip rectangle
    ImGui::GetWindowDrawList()->PushClipRect(ImVec2(currentPosition.x, currentPosition.y), ImVec2(currentPosition.x + m_width, currentPosition.y + lineHeight));
    // Draw the background rectangle
    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(currentPosition.x, currentPosition.y), ImVec2(currentPosition.x + m_width, currentPosition.y + lineHeight),
                                              m_backgroundColor);
    // Deactivate clip rectangle
    ImGui::GetWindowDrawList()->PopClipRect();
}

void TextBox::drawCursor() {
    m_cursor->calculateWidth();

    auto cursorPosition = m_cursor->getCursorPosition(ImGui::GetCursorScreenPos());

    cursorPosition.x -= m_scroll->getXScroll();
    cursorPosition.y -= m_scroll->getYScroll();

    ImGui::GetWindowDrawList()->AddRectFilled(cursorPosition, ImVec2(cursorPosition.x + m_cursor->getWidth(), cursorPosition.y + ImGui::GetFontSize()), ImColor(255, 0, 0));
}


void TextBox::updateTextBoxSize() {
    bool changedWidth = false;
    bool changedHeight = false;

    auto newWidth = ImGui::GetWindowWidth() - m_margin;
    auto newHeight = ImGui::GetWindowHeight() - m_margin;

    if (m_width != newWidth) {
        m_width = newWidth;
        changedWidth = true;
    }

    if (m_height != newHeight) {
        m_height = newHeight;
        changedHeight = true;
    }

    if (changedWidth)
        m_scroll->updateMaxXScroll(m_width);
    if (changedHeight)
        m_scroll->updateMaxYScroll(m_height);
}

bool TextBox::deleteSelection() {
    if (!m_selection->getActive())
        return false;

    auto startIndex = m_lineBuffer->textCoordinatesToBufferIndex(m_selection->getStart());
    auto endIndex = m_lineBuffer->textCoordinatesToBufferIndex(m_selection->getEnd());
    std::cerr << "Deleting from index " << startIndex << " to index " << endIndex << "." << std::endl;
    m_pieceTable->deleteText(startIndex, endIndex);

    m_lineBuffer->getLines();
    m_scroll->updateMaxScroll(m_width, m_height);
    m_cursor->setCoords(m_selection->getStart());

    m_selection->setActive(false);

    return true;
}

TextCoordinates TextBox::mousePositionToTextCoordinates(const ImVec2 &mousePosition) {
    ImGui::PushFont(m_font->getFont());

    float rows = (mousePosition.y - 10.0f) / ImGui::GetFontSize();
    int maxValue = m_lineBuffer->isEmpty() ? 1 : m_lineBuffer->getSize() - 1;
    size_t row = std::min(maxValue, (int) rows);
    auto line = m_lineBuffer->lineAt(row);

    size_t column = 1;

    float xCoords = ImGui::GetCursorScreenPos().x - 5.0f;
    size_t i = 0;
    while (i < line.size()) {
        auto advance = ImGui::GetFont()->GetCharAdvance(line.at(i));
        if (mousePosition.x < xCoords + advance) {
            break;
        }
        ++column;
        xCoords += advance;
        ++i;
    }

    ImGui::PopFont();

    return TextCoordinates(row+1, column);
}







