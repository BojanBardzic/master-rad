//
// Created by bbard on 7/4/2024.
//

#include "LineBuffer.h"

std::string LineBuffer::m_emptyLine;
std::vector<ThemeColor> LineBuffer::m_emptyMap;

LineBuffer::LineBuffer(PieceTableInstance *pieceTableInstance) : m_pieceTableInstance(pieceTableInstance), m_mode(LanguageMode::PlainText) {
    m_lines = new std::vector<std::string>();
    m_colorMap = new std::vector<std::vector<ThemeColor>>();
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

    if (m_mode != LanguageMode::PlainText)
        updateColorMap();
    updateCharSize();
}

size_t LineBuffer::textCoordinatesToBufferIndex(const TextCoordinates &coords) const {
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
    if (index < m_lines->size())
        return m_lines->at(index);
    else
        return m_emptyLine;
}

std::vector<ThemeColor> &LineBuffer::getColorMap(size_t index) const {
    if (index < m_colorMap->size())
        return m_colorMap->at(index);
    else
        return m_emptyMap;
}

bool LineBuffer::lineStarsWithTab(const size_t lineIndex) const {
    if (isEmpty() || lineIndex >= getLinesSize() || m_lines->at(lineIndex).empty())
        return false;
    else
        return m_lines->at(lineIndex).at(0) == '\t';
}

const size_t LineBuffer::getLinesSize() const { return m_lines->size(); }

const size_t LineBuffer::getCharSize() const { return m_charSize; }

const LanguageMode LineBuffer::getLanguageMode() const { return m_mode; }

bool LineBuffer::isEmpty() const { return m_lines->empty(); }

void LineBuffer::setLanguageMode(const LanguageMode mode) { m_mode = mode; }

void LineBuffer::updateCharSize() {
    m_charSize = std::accumulate(m_lines->begin(), m_lines->end(), (size_t) 0, [](size_t acc, std::string& line) { return  acc + line.size() + 1; });
    if (m_charSize != 0)
        m_charSize -= 1;
}

void LineBuffer::updateColorMap() {
    m_colorMap->clear();

    for (std::string& line : *m_lines) {
        m_colorMap->push_back(TextHighlighter::getColorMap(line, m_mode));
    }

    markMultilineComments();
}

void LineBuffer::markMultilineComments() {
    bool insideComment = false;
    const std::string& multilineCommentStart = LanguageManager::getLanguage(m_mode)->getMultiLineCommentStart();
    const std::string& multilineCommentEnd = LanguageManager::getLanguage(m_mode)->getMultiLineCommentEnd();

    for (size_t i=0; i<m_lines->size(); ++i) {
        std::string& line = m_lines->at(i);
        std::vector<ThemeColor>& colorMap = m_colorMap->at(i);

        if (insideComment) {
            auto start = line.find(multilineCommentEnd);

            if (start != std::string::npos) {
                std::fill(colorMap.begin(), colorMap.begin() + start + multilineCommentEnd.size(), ThemeColor::CommentColor);
                insideComment = false;
            } else {
                std::fill(colorMap.begin(), colorMap.end(), ThemeColor::CommentColor);
            }
        } else {
            auto start = line.find(multilineCommentStart);
            auto end = line.find(multilineCommentEnd);

            if (start != std::string::npos && colorMap[start] != ThemeColor::StringColor && colorMap[start] != ThemeColor::CommentColor) {
                if (end != std::string::npos)
                    std::fill(colorMap.begin() + start, colorMap.begin() + end + multilineCommentEnd.size(), ThemeColor::CommentColor);
                else
                    std::fill(colorMap.begin() + start, colorMap.end(), ThemeColor::CommentColor);
                insideComment = true;
            }
        }
    }
}




