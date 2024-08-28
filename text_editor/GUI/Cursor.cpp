//
// Created by bbard on 6/26/2024.
//

#include "Cursor.h"

Cursor::Cursor(LineBuffer* lineBuffer, size_t row, size_t col)
    : m_lineBuffer(lineBuffer), m_timestamp(std::chrono::system_clock::now()),
      m_shouldRender(true), m_position(TextCoordinates(row, col), lineBuffer) {
    calculateWidth();
}

Cursor::~Cursor() {}

void Cursor::moveRight(size_t times) {
    auto moved = m_position.moveRight(times);

    if (moved)
        resetTimer();
}

void Cursor::moveLeft(size_t times) {
    auto moved = m_position.moveLeft(times);

    if (moved)
        resetTimer();
}

void Cursor::moveUp(size_t times) {
    auto moved = m_position.moveUp(times);

    if (moved)
        resetTimer();
}

void Cursor::moveDown(size_t times) {
   auto moved = m_position.moveDown(times);

   if (moved)
       resetTimer();
}

void Cursor::moveToBeginning() {
    auto moved = m_position.moveToBeginningOfRow();

    if (moved)
        resetTimer();
}

void Cursor::moveToEnd() {
    auto moved = m_position.moveToEndOfRow();

    if (moved)
        resetTimer();
}

void Cursor::moveToEndOfFile() {
    auto moved = m_position.moveToEndOfFile();

    if (moved)
        resetTimer();
}

// Records the current cursor Position for undo and redo operations
void Cursor::recordCursorPosition() {
    m_cursorUndoStack.push(m_position.getCoords());
}

// Returns the previously saved cursor position and puts it into the redo stack
void Cursor::cursorUndo() {
    if (!m_cursorUndoStack.empty()) {
        auto coords = m_cursorUndoStack.top();
        m_cursorUndoStack.pop();
        m_cursorRedoStack.push(m_position.getCoords());
        m_position.setCoords(coords);
    }
}

// Returns the previously saved cursor position and puts it into the undo stack
void Cursor::cursorRedo() {
    if (!m_cursorRedoStack.empty()) {
        auto coords = m_cursorRedoStack.top();
        m_cursorRedoStack.pop();
        m_cursorUndoStack.push(m_position.getCoords());
        m_position.setCoords(coords);
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
    auto line = m_lineBuffer->lineAt(m_position.getCoords().m_row-1);
    auto offset = std::min(m_position.getCoords().m_col-1, line.size());

    advance += std::accumulate(line.begin(), line.begin()+offset, 0.0f, [](float acc, char c) { return acc + ImGui::GetFont()->GetCharAdvance(c); });

    return advance;
}

const size_t& Cursor::getRow() const { return m_position.getCoords().m_row; }

const size_t& Cursor::getCol() const { return m_position.getCoords().m_col; }

const TextCoordinates& Cursor::getCoords() const { return m_position.getCoords(); }

const float& Cursor::getWidth() const { return m_width; }

bool Cursor::getShouldRender() const { return m_shouldRender; }

void Cursor::setWidth(const float& width) { m_width = width; }

void Cursor::setRow(const size_t& row) {
    m_shouldRender = true;
    if (row != 0)
        m_position.setRow(row);
}

void Cursor::setCol(const size_t& col) {
    m_shouldRender = true;
    if (col != 0)
        m_position.setColumn(col);
}

void Cursor::setCoords(const TextCoordinates &coords) { m_position.setCoords(coords); }

void Cursor::clipCursor(const TextCoordinates &start, const TextCoordinates &end) {
    m_position.clip(start, end);
}

ImVec2 Cursor::getCursorPosition(const ImVec2& cursorScreenPosition) {
    auto coords = m_position.getCoords();

    auto yOffset = (coords.m_row - 1) * ImGui::GetFontSize();

    auto line = m_lineBuffer->lineAt(coords.m_row-1);
    auto length = std::min(line.size(), coords.m_col-1);

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








