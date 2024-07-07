//
// Created by bbard on 7/4/2024.
//

#ifndef TEXT_EDITOR_LINEBUFFER_H
#define TEXT_EDITOR_LINEBUFFER_H

#include <sstream>
#include "../PieceTable/PieceTable.h"
#include "TextCoordinates.h"

class LineBuffer {
public:
    LineBuffer(PieceTable* pieceTable);
    ~LineBuffer();

    void getLines();
    size_t textCoordinatesToBufferIndex(const TextCoordinates& coords);

    std::string& lineAt(size_t index) const;
    size_t getSize() const;
    bool isEmpty() const;

private:
    std::vector<std::string>* m_lines;
    PieceTable* m_pieceTable;
};


#endif //TEXT_EDITOR_LINEBUFFER_H
