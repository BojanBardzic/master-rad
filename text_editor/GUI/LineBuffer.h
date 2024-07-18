//
// Created by bbard on 7/4/2024.
//

#ifndef TEXT_EDITOR_LINEBUFFER_H
#define TEXT_EDITOR_LINEBUFFER_H

#include <numeric>
#include <sstream>
#include "../PieceTable/PieceTable.h"
#include "TextCoordinates.h"

class LineBuffer {
public:
    LineBuffer(PieceTable* pieceTable);
    ~LineBuffer();

    void getLines();
    size_t textCoordinatesToBufferIndex(const TextCoordinates& coords);
    TextCoordinates bufferIndexToTextCoordinates(const size_t& index);

    std::string& lineAt(size_t index) const;
    const size_t getLinesSize() const;
    const size_t getCharSize() const;
    bool isEmpty() const;

private:
    void updateCharSize();

    static std::string m_emptyLine;
    size_t m_charSize;
    std::vector<std::string>* m_lines;
    PieceTable* m_pieceTable;
};


#endif //TEXT_EDITOR_LINEBUFFER_H
