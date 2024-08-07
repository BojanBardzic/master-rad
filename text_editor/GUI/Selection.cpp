//
// Created by bbard on 7/4/2024.
//

#include "Selection.h"

Selection::Selection(LineBuffer *lineBuffer, size_t startRow, size_t startCol, size_t endRow, size_t endCol)
    : m_lineBuffer(lineBuffer), m_start(TextCoordinates(startRow, startCol)), m_end(TextCoordinates(endRow, endCol)), m_active(false) {}

Selection::~Selection() {}

void Selection::update(TextCoordinates& oldCoords, TextCoordinates& newCoords) {
    if (!m_active) {
        m_active = true;

        bool inOrder = oldCoords < newCoords;
        m_start = inOrder ? oldCoords : newCoords;
        m_end = inOrder ? newCoords : oldCoords;
    } else if (oldCoords == m_start) {
        m_start = newCoords;
    } else if (oldCoords == m_end) {
        m_end = newCoords;
    }

    if (m_start > m_end) {
        auto tmp = m_start;
        m_start = m_end;
        m_end = tmp;
    }
}

bool Selection::getActive() const { return m_active; }

const TextCoordinates& Selection::getStart() const { return m_start; }

const TextCoordinates& Selection::getEnd() const { return m_end; }

void Selection::setActive(bool active) { m_active = active; }

void Selection::setStart(const TextCoordinates& coords) { m_start = coords; }

void Selection::setEnd(const TextCoordinates& coords) { m_end = coords; }

std::pair<size_t, size_t> Selection::getIntersectionWithLine(size_t lineIndex) {
    if (m_lineBuffer->isEmpty())
        return {0, 0};

    auto line = m_lineBuffer->lineAt(lineIndex);
    auto lineRow = lineIndex+1;

    if (m_start.m_row < lineRow && m_end.m_row > lineRow) {
        return {0, line.size()};
    } else if (m_start.m_row == lineRow && m_end.m_row == lineRow) {
        return {m_start.m_col-1, m_end.m_col-1};
    } else if (m_start.m_row == lineRow && m_end.m_row > lineRow) {
        return {m_start.m_col-1, line.size()};
    } else if (m_start.m_row < lineRow && m_end.m_row == lineRow) {
        return {0, m_end.m_col-1};
    } else {
        return {0, 0};
    }
}

void Selection::selectAll(TextCoordinates& newCoords) {
    if (!m_active) {
        m_active = true;
    }

    m_start.m_row = 1;
    m_start.m_col = 1;
    m_end = newCoords;
}

void Selection::moveEndRight() {
    auto line = m_lineBuffer->lineAt(m_end.m_row-1);

    if (!line.empty())
        m_end.m_col = std::min(m_end.m_col+1, line.size()+1);
}

void Selection::moveEndLeft() {
    m_end.m_col = std::max(m_end.m_col-1, (size_t) 1);
}

std::string Selection::getSelectionText() {
    std::string result;
    TextCoordinates it = m_start;

    std::string* line = &m_lineBuffer->lineAt(it.m_row-1);

    while (it < m_end) {
        if (it.m_col > line->size()) {
            result += '\n';
            it.m_row += 1;
            it.m_col = 1;
            line = &m_lineBuffer->lineAt(it.m_row-1);
        } else {
            if (!line->empty())
                result.push_back(line->at(it.m_col-1));

            it.m_col += 1;
        }
    }

    return result;
}




