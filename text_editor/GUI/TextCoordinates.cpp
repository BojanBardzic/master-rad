//
// Created by bbard on 7/4/2024.
//

#include "TextCoordinates.h"

TextCoordinates& TextCoordinates::operator=(const TextCoordinates &other) {
    this->m_row = other.m_row;
    this->m_col = other.m_col;
    return *this;
}

bool TextCoordinates::operator==(const TextCoordinates &other) {
    return this->m_row == other.m_row && this->m_col == other.m_col;
}

bool TextCoordinates::operator!=(const TextCoordinates &other) {
    return this->m_row != other.m_row || this->m_col == other.m_col;
}

bool TextCoordinates::operator<(const TextCoordinates &other) {
    return (m_row < other.m_row) || (m_row == other.m_row && m_col < other.m_col);
}

bool TextCoordinates::operator>(const TextCoordinates &other) {
    return (m_row > other.m_row) || (m_row == other.m_row && m_col > other.m_col);
}

TextCoordinates::TextCoordinates(size_t row, size_t col) : m_row(row), m_col(col) {}

TextCoordinates::~TextCoordinates() {}






