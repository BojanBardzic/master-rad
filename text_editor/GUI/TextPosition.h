//
// Created by bbard on 8/4/2024.
//

#ifndef TEXT_EDITOR_TEXTPOSITION_H
#define TEXT_EDITOR_TEXTPOSITION_H

#include "LineBuffer.h"
#include "TextCoordinates.h"

class TextPosition {
public:
    TextPosition(TextCoordinates coords, LineBuffer* lineBuffer);
    ~TextPosition();

    bool moveRight(size_t times = 1);
    bool moveLeft(size_t times = 1);
    bool moveUp(size_t times = 1);
    bool moveDown(size_t times = 1);
    bool moveToBeginningOfRow();
    bool moveToEndOfRow();
    bool moveToEndOfFile();

    void clip(const TextCoordinates& start, const TextCoordinates& end);

    const TextCoordinates& getCoords() const;
    void setCoords(const TextCoordinates& coords);
    void setRow(const size_t& row);
    void setColumn(const size_t& column);
private:
    void correctColumn();

    bool isOnBeginningOfLine();
    bool isOnEndOfLine();
    bool isOnFirstLine();
    bool isOnLastLine();

    TextCoordinates m_coords;
    LineBuffer* m_lineBuffer;
};


#endif //TEXT_EDITOR_TEXTPOSITION_H
