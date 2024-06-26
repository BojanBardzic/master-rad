//
// Created by bbard on 6/26/2024.
//

#include "Cursor.h"

Cursor::Cursor(size_t row, size_t col) : m_row(row), m_col(col) {
    calculateWidth();
}

Cursor::~Cursor() {}

void Cursor::draw(ImVec2 cursorScreenPosition, std::string& line) {
    calculateWidth();

    ImGui::BeginChild("Child");
    ImGui::BeginChild("Cursor");

    auto cursorPosition = getCursorPosition(cursorScreenPosition, line);
    ImGui::GetWindowDrawList()->AddRectFilled(cursorPosition, ImVec2(cursorPosition.x + m_width, cursorPosition.y + ImGui::GetFontSize()), ImColor(255, 0, 0));

    ImGui::EndChild();
    ImGui::EndChild();
}

size_t Cursor::getRow() const { return m_row; }

size_t Cursor::getCol() const { return m_col; }

void Cursor::setRow(size_t row) { m_row = row; }

void Cursor::setCol(size_t col) { m_col = col; }

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


