//
// Created by bbard on 6/26/2024.
//

#include "Cursor.h"

Cursor::Cursor(size_t row, size_t col)
    : m_row(row), m_col(col), m_timestamp(std::chrono::system_clock::now()), m_shouldRender(true)
    {
    calculateWidth();
}

Cursor::~Cursor() {}

size_t Cursor::getRow() const { return m_row; }

size_t Cursor::getCol() const { return m_col; }

float Cursor::getWidth() const { return m_width; }

std::chrono::time_point<std::chrono::system_clock> Cursor::getTimestamp() const { return m_timestamp; }

bool Cursor::getShouldRender() const { return m_shouldRender; }

void Cursor::setRow(size_t row) {
    m_shouldRender = true;
    m_row = row;
}

void Cursor::setCol(size_t col) {
    m_shouldRender = true;
    m_col = col;
}

void Cursor::setWidth(float width) { m_width = width; }

void Cursor::setTimestamp(std::chrono::time_point<std::chrono::system_clock> timestamp) { m_timestamp = timestamp; }

ImVec2 Cursor::getCursorPosition(ImVec2 cursorScreenPosition, std::string& line) {
    auto xOffset = 0.0f;
    auto yOffset = (m_row - 1) * ImGui::GetFontSize();

    for (int i=0; i<m_col-1; ++i) {
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






