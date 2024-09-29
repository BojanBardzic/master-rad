//
// Created by bbard on 8/4/2024.
//

#include "TextPosition.h"

TextPosition::TextPosition(TextCoordinates coords, LineBuffer *lineBuffer) : m_coords(coords), m_lineBuffer(lineBuffer) {}

TextPosition::~TextPosition() {}

bool TextPosition::moveRight(size_t times) {
    bool moved = false;

    for (size_t i=0; i<times; ++i) {

        if (m_lineBuffer->isEmpty() || (isOnEndOfLine() && isOnLastLine())) {
            return moved;
        }

        moved = true;

        if (isOnEndOfLine()) {
            m_coords.m_col = 1;
            moveDown();
        } else {
            m_coords.m_col++;
        }
    }

    return moved;
}

bool TextPosition::moveLeft(size_t times) {
    bool moved = false;

    for (size_t i=0; i<times; ++i) {

        if (m_lineBuffer->isEmpty() || (isOnBeginningOfLine() && isOnFirstLine())) {
            return moved;
        }

        moved = true;

        if (isOnBeginningOfLine()) {
            m_coords.m_col = m_lineBuffer->lineAt(m_coords.m_row - 2).size() + 1;
            moveUp();
        } else {
            m_coords.m_col--;
        }
    }

    return moved;
}

bool TextPosition::moveUp(size_t times) {
    bool moved = false;
    size_t i = 0;

    while (!isOnFirstLine() && i < times) {
        m_coords.m_row--;
        moved = true;
        correctColumn();
        ++i;
    }

    return moved;
}

bool TextPosition::moveDown(size_t times) {
    bool moved = false;
    size_t i = 0;

    while (!m_lineBuffer->isEmpty() && !isOnLastLine() && i < times) {
        m_coords.m_row++;
        moved = true;
        correctColumn();
        i++;
    }

    return moved;
}

bool TextPosition::moveToBeginningOfRow() {
    m_coords.m_col = 1;
    return true;
}

bool TextPosition::moveToEndOfRow() {
    if (!m_lineBuffer->isEmpty()) {
        m_coords.m_col = m_lineBuffer->lineAt(m_coords.m_row - 1).size() + 1;
    }
    return true;
}

bool TextPosition::moveToEndOfFile() {
    m_coords.m_row = m_lineBuffer->getLinesSize();
    m_coords.m_col = m_lineBuffer->lineAt(m_coords.m_row-1).size() + 1;
    return true;
}

void TextPosition::clip(const TextCoordinates &start, const TextCoordinates &end) {
    if (m_coords < start)
        m_coords = start;
    if (m_coords > end)
        m_coords = end;
}

const TextCoordinates &TextPosition::getCoords() const { return m_coords; }

void TextPosition::setCoords(const TextCoordinates &coords) { m_coords = coords; }

void TextPosition::setRow(const size_t &row) { m_coords.m_row = row; }

void TextPosition::setColumn(const size_t &column) { m_coords.m_col = column; }

void TextPosition::correctColumn() {
    if (!m_lineBuffer->isEmpty())
        m_coords.m_col = std::min(m_coords.m_col, m_lineBuffer->lineAt(m_coords.m_row-1).size() + 1);
}

bool TextPosition::isOnBeginningOfLine() {
    return m_coords.m_col == 1;
}

bool TextPosition::isOnEndOfLine() {
    return m_coords.m_col > m_lineBuffer->lineAt(m_coords.m_row-1).size();
}

bool TextPosition::isOnFirstLine() {
    return m_coords.m_row == 1;
}

bool TextPosition::isOnLastLine() {
    return m_coords.m_row == m_lineBuffer->getLinesSize();
}
