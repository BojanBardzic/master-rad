//
// Created by bbard on 7/4/2024.
//

#ifndef TEXT_EDITOR_SELECTION_H
#define TEXT_EDITOR_SELECTION_H

#include "TextPosition.h"
#include "LineBuffer.h"

class Selection {
public:
    Selection(LineBuffer* lineBuffer, size_t startRow = 1, size_t startCol = 1, size_t endRow = 1, size_t endCol = 1);
    ~Selection();

    void update(TextCoordinates& oldCoords, TextCoordinates& newCoords);
    void selectAll(TextCoordinates& newCoords);

    void moveStartRight(size_t times = 1);
    void moveStartLeft(size_t times = 1);
    void moveEndRight(size_t times = 1);
    void moveEndLeft(size_t times = 1);

    std::string getSelectionText();
    std::pair<size_t, size_t> getIntersectionWithLine(size_t lineIndex);
    void clipSelection(const TextCoordinates& start, const TextCoordinates& end);

    bool isActive() const;
    const TextCoordinates& getStart() const;
    const TextCoordinates& getEnd() const;

    void setActive(bool active);
    void setStart(const TextCoordinates& coords);
    void setEnd(const TextCoordinates& coords);
private:

    bool m_active;
    TextPosition m_start;
    TextPosition m_end;
    LineBuffer* m_lineBuffer;
};


#endif //TEXT_EDITOR_SELECTION_H
