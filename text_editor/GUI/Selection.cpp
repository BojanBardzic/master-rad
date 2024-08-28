//
// Created by bbard on 7/4/2024.
//

#include "Selection.h"

Selection::Selection(LineBuffer *lineBuffer, size_t startRow, size_t startCol, size_t endRow, size_t endCol)
    : m_lineBuffer(lineBuffer), m_start(TextCoordinates(startRow, startCol), lineBuffer), m_end(TextCoordinates(endRow, endCol), lineBuffer), m_active(false) {}

Selection::~Selection() {}

void Selection::update(TextCoordinates& oldCoords, TextCoordinates& newCoords) {
    if (!m_active) {
        m_active = true;

        bool inOrder = oldCoords < newCoords;

        m_start.setCoords(inOrder ? oldCoords : newCoords);
        m_end.setCoords(inOrder ? newCoords : oldCoords);
    } else if (oldCoords == m_start.getCoords()) {
        m_start.setCoords(newCoords);
    } else if (oldCoords == m_end.getCoords()) {
        m_end.setCoords(newCoords);
    }

    TextCoordinates start = m_start.getCoords();
    TextCoordinates end = m_end.getCoords();

    if (start > end) {
        auto tmp = m_start.getCoords();
        m_start.setCoords(m_end.getCoords());
        m_end.setCoords(tmp);
    }
}

void Selection::selectAll(TextCoordinates& newCoords) {
    if (!m_active) {
        m_active = true;
    }

    m_start.setCoords({1, 1});
    m_end.setCoords(newCoords);
}

void Selection::moveStartRight(size_t times) {
    m_start.moveRight(times);

    if (m_start.getCoords() > m_end.getCoords())
        m_start.setCoords(m_end.getCoords());
}

void Selection::moveStartLeft(size_t times) {
    m_start.moveLeft(times);
}

void Selection::moveEndRight(size_t times) {
    m_end.moveRight(times);
}

void Selection::moveEndLeft(size_t times) {
    m_end.moveLeft(times);

    if (m_end.getCoords() < m_start.getCoords())
        m_end.setCoords(m_start.getCoords());
}



std::string Selection::getSelectionText() {
    std::string result;
    TextCoordinates it = m_start.getCoords();

    std::string* line = &m_lineBuffer->lineAt(it.m_row-1);

    while (it < m_end.getCoords()) {
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

std::pair<size_t, size_t> Selection::getIntersectionWithLine(size_t lineIndex) {
    if (m_lineBuffer->isEmpty())
        return {0, 0};

    auto line = m_lineBuffer->lineAt(lineIndex);
    auto lineRow = lineIndex+1;

    auto start = m_start.getCoords();
    auto end = m_end.getCoords();

    if (start.m_row < lineRow && end.m_row > lineRow) {
        return {0, line.size()};
    } else if (start.m_row == lineRow && end.m_row == lineRow) {
        return {start.m_col-1, end.m_col-1};
    } else if (start.m_row == lineRow && end.m_row > lineRow) {
        return {start.m_col-1, line.size()};
    } else if (start.m_row < lineRow && end.m_row == lineRow) {
        return {0, end.m_col-1};
    } else {
        return {0, 0};
    }
}

void Selection::clipSelection(const TextCoordinates &start, const TextCoordinates &end) {
    m_start.clip(start, end);
    m_end.clip(start, end);
}

bool Selection::isActive() const { return m_active; }

const TextCoordinates& Selection::getStart() const { return m_start.getCoords(); }

const TextCoordinates& Selection::getEnd() const { return m_end.getCoords(); }

void Selection::setActive(bool active) { m_active = active; }

void Selection::setStart(const TextCoordinates& coords) { m_start.setCoords(coords); }

void Selection::setEnd(const TextCoordinates& coords) { m_end.setCoords(coords); }





