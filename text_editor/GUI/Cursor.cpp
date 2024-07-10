//
// Created by bbard on 6/26/2024.
//

#include "Cursor.h"

Cursor::Cursor(LineBuffer* lineBuffer, size_t row, size_t col)
    : m_lineBuffer(lineBuffer), m_timestamp(std::chrono::system_clock::now()),
      m_shouldRender(true), m_coord(TextCoordinates(row, col)) {
    calculateWidth();
}

Cursor::~Cursor() {}

void Cursor::moveRight() {
    if (m_lineBuffer->isEmpty() || (m_coord.m_col > m_lineBuffer->lineAt(m_coord.m_row-1).size() && m_coord.m_row == m_lineBuffer->getSize())) {
        return;
    }

    resetTimer();

    if (m_coord.m_col > m_lineBuffer->lineAt(m_coord.m_row-1).size()) {
        m_coord.m_col = 1;
        moveDown();
    } else {
        m_coord.m_col += 1;
    }
}

void Cursor::moveLeft() {
    if (m_lineBuffer->isEmpty() || m_coord.m_col == 1 && m_coord.m_row == 1) {
        return;
    }

    resetTimer();

    if (m_coord.m_col == 1) {
        m_coord.m_col = m_lineBuffer->lineAt(m_coord.m_row-2).size() + 1;
        moveUp();
    } else {
        m_coord.m_col -= 1;
    }
}

void Cursor::moveUp() {
    if (!m_lineBuffer->isEmpty() && m_coord.m_row != 1) {
        resetTimer();
        m_coord.m_row -= 1;
        correctColumn();
    }
}

void Cursor::moveDown() {
    if (!m_lineBuffer->isEmpty() && m_coord.m_row != m_lineBuffer->getSize()) {
        resetTimer();
        m_coord.m_row += 1;
        correctColumn();
    }
}

void Cursor::moveToBeginning() { m_coord.m_col = 1; }

void Cursor::moveToEnd() {
    if (!m_lineBuffer->isEmpty()) {
        m_coord.m_col = m_lineBuffer->lineAt(m_coord.m_row - 1).size() + 1;
    }
}

void Cursor::moveToEndOfFile() {
    m_coord.m_row = m_lineBuffer->getSize();
    m_coord.m_col = m_lineBuffer->lineAt(m_coord.m_row-1).size() + 1;
}

// Calculates the x-axis advancement of the substring of the line at cursorRow-1
// between [0, cursorCol)
float Cursor::getXAdvance() const {
    float advance = 0.0f;
    auto line = m_lineBuffer->lineAt(m_coord.m_row-1);

    if (m_coord.m_col == 1)
        return 0.0f;

    for (size_t i = 0; i<m_coord.m_col; ++i) {
        advance += ImGui::GetFont()->GetCharAdvance(line[i]);
    }

    return advance;
}

size_t Cursor::getRow() const { return m_coord.m_row; }

size_t Cursor::getCol() const { return m_coord.m_col; }

TextCoordinates Cursor::getCoords() const { return m_coord; }

float Cursor::getWidth() const { return m_width; }

bool Cursor::getShouldRender() const { return m_shouldRender; }

void Cursor::setRow(size_t row) {
    m_shouldRender = true;
    if (row != 0)
        m_coord.m_row = row;
}

void Cursor::setCol(size_t col) {
    m_shouldRender = true;
    if (col != 0)
        m_coord.m_col = col;
}

void Cursor::setCoords(const TextCoordinates &coords) { m_coord = coords; }

void Cursor::setWidth(float width) { m_width = width; }

ImVec2 Cursor::getCursorPosition(ImVec2 cursorScreenPosition) {
    auto xOffset = 0.0f;
    auto yOffset = (m_coord.m_row - 1) * ImGui::GetFontSize();

    auto line = m_lineBuffer->lineAt(m_coord.m_row-1);

    for (int i=0; i<m_coord.m_col-1; ++i) {
        xOffset += ImGui::GetFont()->GetCharAdvance(line[i]);
    }

    return ImVec2(cursorScreenPosition.x + xOffset, cursorScreenPosition.y + yOffset);
}

void Cursor::calculateWidth() {
    m_width = 0.1f * ImGui::GetFontSize();
}

void Cursor::updateShouldRender() {
    auto now = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed = now - m_timestamp;

    if (elapsed.count() >= m_drawInterval.count()) {
        m_timestamp = now;
        m_shouldRender = !m_shouldRender;
    }
}

float Cursor::getXAdvance(std::string& str) {
    float advance = 0.0f;

    for (const char& c : str) {
        advance += ImGui::GetFont()->GetCharAdvance(c);
    }

    return advance;
}

void Cursor::resetTimer() {
    m_shouldRender = true;
    m_timestamp = std::chrono::system_clock::now();
}

void Cursor::correctColumn() {
    if (m_coord.m_col > m_lineBuffer->lineAt(m_coord.m_row-1).size() + 1) {
        moveToEnd();
    }
}






