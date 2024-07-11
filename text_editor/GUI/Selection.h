//
// Created by bbard on 7/4/2024.
//

#ifndef TEXT_EDITOR_SELECTION_H
#define TEXT_EDITOR_SELECTION_H

#include "TextCoordinates.h"
#include "LineBuffer.h"

class Selection {
public:
    Selection(LineBuffer* lineBuffer, size_t startRow = 1, size_t startCol = 1, size_t endRow = 1, size_t endCol = 1);
    ~Selection();

    void update(TextCoordinates& oldCoords, TextCoordinates& newCoords);
    void selectAll(TextCoordinates& newCoords);

    std::string getSelectionText();

    bool getActive() const;
    const TextCoordinates& getStart() const;
    const TextCoordinates& getEnd() const;

    void setActive(bool active);
    void setStart(const TextCoordinates& coords);
    void setEnd(const TextCoordinates& coords);

    std::pair<size_t, size_t> getIntersectionWithLine(size_t lineIndex);
private:

    bool m_active;
    TextCoordinates m_start;
    TextCoordinates m_end;
    LineBuffer* m_lineBuffer;
};


#endif //TEXT_EDITOR_SELECTION_H
