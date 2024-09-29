//
// Created by bbard on 7/4/2024.
//

#include "LineBuffer.h"

std::string LineBuffer::m_emptyLine;
std::vector<ThemeColor> LineBuffer::m_emptyMap;

LineBuffer::LineBuffer(PieceTableInstance *pieceTableInstance) : m_pieceTableInstance(pieceTableInstance), m_mode(LanguageMode::PlainText) {
    m_lines = new std::vector<std::string>();
    m_colorMap = new std::vector<std::vector<ThemeColor>>();
    m_blocks = new std::vector<CodeBlock*>();
    m_hidden = new std::vector<bool>();
}

LineBuffer::~LineBuffer() {
    delete m_lines;
    delete m_blocks;
    delete m_hidden;
}

// Turns PieceTable data into lines.
void LineBuffer::getLines(int lineIndex) {
    // Set up a string stream and load the PieceTable data into it
    std::stringstream stream;
    stream << m_pieceTableInstance->getInstance();
    // Turn the string stream into a std::string
    std::string textBuffer = stream.str();

    // Save the old size
    int oldSize = m_lines->size();
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

    if (m_mode != LanguageMode::PlainText) {
        updateColorMap();
        updateBlocks(m_lines->size()-oldSize, lineIndex);
    }
    updateCharSize();
}

// Updates the vector of hidden lines with the new state of the block
void LineBuffer::updateHiddenForBlock(CodeBlock *block) {
    writeInHidden(block);

    // If we have unfolded a block that has folded blocks inside it
    // we need to keep those lines hidden
    if (!block->isFolded()) {
        // We find the current block
        auto index = findBlock(block);

        // If we found it we iterate through blocks inside it
        if (index != -1) {
            index++;
            while (index < m_blocks->size() && m_blocks->at(index)->getStart().m_row <= block->getEnd().m_row) {
                writeInHidden(m_blocks->at(index));
                index++;
            }
        }
    }
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

const std::vector<CodeBlock*>& LineBuffer::getBlocks() const { return *m_blocks; }

const std::vector<bool>& LineBuffer::getHidden() const { return *m_hidden; }

// Gets how many rows are shown up to lineIndex
const size_t LineBuffer::getRowsShowing(size_t lineIndex) const {
//    lineIndex = std::min(lineIndex, m_hidden->size());
//
//    size_t rowsShowing = std::accumulate(m_hidden->begin(), m_hidden->begin() + lineIndex, 0,
//                             [](int acc, bool b) {
//                                 if (!b)
//                                     return acc + 1;
//
//                                 return acc;
//                             }
//                        );

    size_t rowsShowing = lineIndex;
    size_t currentBlockIndex = 0;

    // Iterate through the blocks that begin before the lineIndex
    while (currentBlockIndex < m_blocks->size() && m_blocks->at(currentBlockIndex)->getStart().m_row < lineIndex) {

        if (m_blocks->at(currentBlockIndex)->isFolded()) {
            auto foldedBlock = m_blocks->at(currentBlockIndex);
            // If the block is folded we calculate the number of lines that will not be shown
            auto offset = std::min(lineIndex, foldedBlock->getEnd().m_row) - foldedBlock->getStart().m_row;
            rowsShowing -= offset;

            // We go to the next block and skip until we find one that wasn't inside the folded block
            do {
                currentBlockIndex++;
            } while (currentBlockIndex < m_blocks->size() && m_blocks->at(currentBlockIndex)->getStart().m_row <= foldedBlock->getEnd().m_row);
        } else {
            // In case it isn't folded we just go to the next block
            currentBlockIndex++;
        }
    }

    return rowsShowing;
}

bool LineBuffer::lineStarsWithTab(const size_t lineIndex) const {
    if (isEmpty() || lineIndex >= getLinesSize() || m_lines->at(lineIndex).empty())
        return false;
    else
        return m_lines->at(lineIndex).at(0) == '\t';
}

bool LineBuffer::isPlainText() const { return m_mode == LanguageMode::PlainText; }

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



void LineBuffer::updateBlocks(int lineSizeDiff, int lineIndex) {
    auto newBlocks = new std::vector<CodeBlock*>();
    std::stack<TextCoordinates> openBrackets;
    auto size = m_lines->size();

    if (lineSizeDiff != 0)
        updateFoldedBlocks(lineSizeDiff, lineIndex);

    auto oldHidden = m_hidden;
    m_hidden = new std::vector<bool>(size, false);
    delete oldHidden;

    for (size_t i=0; i<size; ++i) {
        auto line = m_lines->at(i);

        for (size_t j=0; j<line.size(); ++j) {
            if (line[j] == '{') {
                openBrackets.push(TextCoordinates(i, j+1));
            } else if (line[j] == '}' && !openBrackets.empty()) {
                auto openBracket = openBrackets.top();
                openBrackets.pop();

                auto closedBracket = TextCoordinates(i, j+1);
                auto block = new CodeBlock(openBracket, closedBracket);

                auto index = findBlock(block);

                if (index != -1 && m_blocks->at(index)->isFolded() == true) {
                    block->setFolded(true);
                    updateHiddenForBlock(block);
                }

                newBlocks->push_back(block);
            }
        }
    }

    auto oldBlocks = m_blocks;
    m_blocks = newBlocks;

    for (auto block : *oldBlocks)
        delete block;
    delete oldBlocks;

    std::sort(m_blocks->begin(), m_blocks->end(), [](CodeBlock* first, CodeBlock* second) { return *first < *second; });
}

void LineBuffer::updateFoldedBlocks(int lineSizeDiff, int lineIndex) {
    if (lineIndex == -1)
        return;

    if (lineSizeDiff < 0)
        lineIndex -= lineSizeDiff;

    auto index = findGreaterOrEqualBlock(lineIndex);

    while (index != -1 && index < m_blocks->size()) {
        auto block = m_blocks->at(index);

        if (block->isFolded()) {
            block->setStart({block->getStart().m_row + lineSizeDiff, block->getStart().m_col});
            block->setEnd({block->getEnd().m_row + lineSizeDiff, block->getEnd().m_col});
        }

        ++index;
    }
}

// Code from https://www.geeksforgeeks.org/cpp-binary-search/
int LineBuffer::findBlock(CodeBlock *block) {
    int low = 0;
    int high = m_blocks->size()-1;
    int mid;

    while (high >= low) {
        // Calculate the middle index to divide the search
        // space in half
        mid = low + (high - low) / 2;

        // If the middle element is equal to 'x', we have
        // found the element, return its index
        if (*(m_blocks->at(mid)) == *block)
            return mid;

        // If the middle element is greater than 'x', search
        // in the left half of the array
        if (*(m_blocks->at(mid)) > *block)
            high = mid-1;
        else
            low = mid+1;
        // If the middle element is less than 'x', search in
        // the right half of the array
    }

    // If the base case is reached, the element is not
    // present in the array, return -1
    return -1;
}

// Finds the index of the first block that is greater or equal to the line number, or returns -1 if there are none
int LineBuffer::findGreaterOrEqualBlock(size_t lineIndex) {
    int result = -1;

    int low = 0;
    int high = m_blocks->size()-1;
    int mid;

    while (low <= high) {
        mid = low + (high - low) / 2;

        if (m_blocks->at(mid)->getStart().m_row >= lineIndex) {
            result = mid;
            high = mid-1;
        } else {
            low = mid+1;
        }
    }

    return result;
}

// Fills the block lines of the hidden vector with the value of isFolded()
void LineBuffer::writeInHidden(CodeBlock *block) {
    std::fill(m_hidden->begin() + block->getStart().m_row + 1, m_hidden->begin() + block->getEnd().m_row + 1,
              block->isFolded());
}

// Marks the occurrence of multiline comments
void LineBuffer::markMultilineComments() {
    bool insideComment = false;
    // Get start and end
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










