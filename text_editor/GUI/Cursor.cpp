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
    if (m_lineBuffer->isEmpty() || (m_coord.m_col > m_lineBuffer->lineAt(m_coord.m_row-1).size() && m_coord.m_row ==
                                                                                                            m_lineBuffer->getLinesSize())) {
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
    if (m_coord.m_row != 1) {
        resetTimer();
        m_coord.m_row -= 1;
        correctColumn();
    }
}

void Cursor::moveDown() {
    if (!m_lineBuffer->isEmpty() && m_coord.m_row < m_lineBuffer->getLinesSize()) {
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
    m_coord.m_row = m_lineBuffer->getLinesSize();
    m_coord.m_col = m_lineBuffer->lineAt(m_coord.m_row-1).size() + 1;
}

// Records the current cursor Position for undo and redo operations
void Cursor::recordCursorPosition() {
    m_cursorUndoStack.push(m_coord);
}

// Returns the previously saved cursor position and puts it into the redo stack
void Cursor::cursorUndo() {
    if (!m_cursorUndoStack.empty()) {
        auto coords = m_cursorUndoStack.top();
        m_cursorUndoStack.pop();
        m_cursorRedoStack.push(m_coord);
        m_coord = coords;
    }
}

// Returns the previously saved cursor position and puts it into the undo stack
void Cursor::cursorRedo() {
    if (!m_cursorRedoStack.empty()) {
        auto coords = m_cursorRedoStack.top();
        m_cursorRedoStack.pop();
        m_cursorUndoStack.push(m_coord);
        m_coord = coords;
    }
}

void Cursor::clearUndoAndRedoStacks() {
    clearUndoStack();
    clearRedoStack();
}

// Calculates the x-axis advancement of the substring of the line at cursorRow-1
// between [0, cursorCol)
const float Cursor::getXAdvance() const {
    float advance = ImGui::GetFont()->GetCharAdvance('W');
    auto line = m_lineBuffer->lineAt(m_coord.m_row-1);
    auto offset = std::min(m_coord.m_col-1, line.size());

    advance += std::accumulate(line.begin(), line.begin()+offset, 0.0f, [](float acc, char c) { return acc + ImGui::GetFont()->GetCharAdvance(c); });

    return advance;
}

const size_t& Cursor::getRow() const { return m_coord.m_row; }

const size_t& Cursor::getCol() const { return m_coord.m_col; }

const TextCoordinates& Cursor::getCoords() const { return m_coord; }

const float& Cursor::getWidth() const { return m_width; }

bool Cursor::getShouldRender() const { return m_shouldRender; }

void Cursor::setRow(const size_t& row) {
    m_shouldRender = true;
    if (row != 0)
        m_coord.m_row = row;
}

void Cursor::setCol(const size_t& col) {
    m_shouldRender = true;
    if (col != 0)
        m_coord.m_col = col;
}

void Cursor::setCoords(const TextCoordinates &coords) { m_coord = coords; }

void Cursor::setWidth(const float& width) { m_width = width; }

ImVec2 Cursor::getCursorPosition(const ImVec2& cursorScreenPosition) {
    auto yOffset = (m_coord.m_row - 1) * ImGui::GetFontSize();

    auto line = m_lineBuffer->lineAt(m_coord.m_row-1);
    auto length = std::min(line.size(), m_coord.m_col-1);

    auto xOffset = getXAdvance(line.substr(0, length));

    return {cursorScreenPosition.x + xOffset, cursorScreenPosition.y + yOffset};
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

float Cursor::getXAdvance(const std::string& str) {
    return std::accumulate(str.begin(), str.end(), 0.0f,
                           [](float acc, char c) { return acc + ImGui::GetFont()->GetCharAdvance(c); });
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

void Cursor::clearUndoStack() {
    while (!m_cursorUndoStack.empty()) {
        m_cursorUndoStack.pop();
    }
}

void Cursor::clearRedoStack() {
    while (!m_cursorRedoStack.empty()) {
        m_cursorRedoStack.pop();
    }
}






