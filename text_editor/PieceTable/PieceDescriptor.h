//
// Created by bbard on 6/9/2024.
//

#ifndef TEXT_EDITOR_PIECEDESCRIPTOR_H
#define TEXT_EDITOR_PIECEDESCRIPTOR_H

#include <vector>
#include <iostream>
#include "SourceType.h"

class PieceDescriptor {
public:
    friend std::ostream& operator<<(std::ostream& out, const PieceDescriptor& piece);

    PieceDescriptor(SourceType source, size_t start, size_t length);
    ~PieceDescriptor();

    SourceType getSource() const;
    size_t getStart() const;
    size_t getLength() const;

    void setSource(SourceType mSource);
    void setStart(size_t mStart);
    void setLength(size_t mLength);

private:
    SourceType m_source;
    size_t m_start;
    size_t m_length;
};


#endif //TEXT_EDITOR_PIECEDESCRIPTOR_H
