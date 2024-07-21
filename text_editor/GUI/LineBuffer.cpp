//
// Created by bbard on 7/4/2024.
//

#include "LineBuffer.h"

std::string LineBuffer::m_emptyLine = "";

LineBuffer::LineBuffer(PieceTableInstance *pieceTableInstance) : m_pieceTableInstance(pieceTableInstance) {
    m_lines = new std::vector<std::string>();
}

LineBuffer::~LineBuffer() {
    delete m_lines;
}

// Turns PieceTable data into lines.
void LineBuffer::getLines() {
    // Set up a string stream and load the PieceTable data into it
    std::stringstream stream;
    stream << m_pieceTableInstance->getInstance();
    // Turn the string stream into a std::string
    std::string textBuffer = stream.str();

    // Clear the previous lines
    m_lines->clear();

    auto it = textBuffer.begin();

    while (it != textBuffer.end()) {
        // Find the newline in the buffer
        auto newLine = std::find(it, textBuffer.end(), '\n');
        // Adds the substring starting from the current iterator unitl the newline character
        m_lines->push_back(textBuffer.substr(it-textBuffer.begin(), newLine-it));

        // If we have reached the end of the buffer we break
        if (newLine == textBuffer.end())
            break;

        // If the last character in the buffer is a newLine add another line at the end
        if (std::next(newLine) == textBuffer.end())
            m_lines->push_back("");

        //We jump over the newline char and iterate to the next.
        it = std::next(newLine);
    }

    updateCharSize();
}

size_t LineBuffer::textCoordinatesToBufferIndex(const TextCoordinates &coords) {
    size_t index = 0;

    for (size_t i=0; i<coords.m_row-1; i++) {
        index += lineAt(i).size() + 1;
    }

    index += (coords.m_col - 1);

    return index;
}

TextCoordinates LineBuffer::bufferIndexToTextCoordinates(const size_t& index) {
    size_t row = 1;
    size_t col = 1;

    if (isEmpty())
        return {row, col};

    size_t acc = 0;
    while (row-1 < m_lines->size() && acc + m_lines->at(row-1).size() < index) {
        acc += m_lines->at(row-1).size() + 1;
        ++row;
    }

    col += index - acc;

    return {row, col};
}

std::string& LineBuffer::lineAt(size_t index) const {
    if (index >= 0 && index < m_lines->size()) {
        return m_lines->at(index);
    } else {
        return m_emptyLine;
    }
}

const size_t LineBuffer::getLinesSize() const { return m_lines->size(); }

const size_t LineBuffer::getCharSize() const { return m_charSize; }

bool LineBuffer::isEmpty() const { return m_lines->empty(); }

void LineBuffer::updateCharSize() {
    m_charSize = std::accumulate(m_lines->begin(), m_lines->end(), (size_t) 0, [](size_t acc, std::string& line) { return  acc + line.size() + 1; });
    if (m_charSize != 0)
        m_charSize -= 1;
}


