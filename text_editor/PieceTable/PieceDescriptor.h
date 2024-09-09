//
// Created by bbard on 6/9/2024.
//

#ifndef TEXT_EDITOR_PIECEDESCRIPTOR_H
#define TEXT_EDITOR_PIECEDESCRIPTOR_H

#include "SourceType.h"

#include <iostream>
#include <vector>

class PieceDescriptor {
public:
    friend std::ostream& operator<<(std::ostream& out, const PieceDescriptor& piece);

    PieceDescriptor(SourceType source, size_t start, size_t length);
    PieceDescriptor(PieceDescriptor& piece);
    PieceDescriptor(PieceDescriptor* piece);
    ~PieceDescriptor();

    SourceType getSource() const;
    size_t getStart() const;
    size_t getLength() const;

    void setSource(SourceType mSource);
    void setStart(size_t mStart);
    void setLength(size_t mLength);

    static PieceDescriptor* cutoffFromRight(PieceDescriptor* piece, size_t offset);
    static PieceDescriptor* cutoffFromLeft(PieceDescriptor* piece, size_t offset);

    static std::pair<PieceDescriptor*, PieceDescriptor*> splitPiece(PieceDescriptor* piece, size_t splitIndex);
private:
    SourceType m_source;
    size_t m_start;
    size_t m_length;
};


#endif //TEXT_EDITOR_PIECEDESCRIPTOR_H
