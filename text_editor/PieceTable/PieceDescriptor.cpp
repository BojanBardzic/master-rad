//
// Created by bbard on 6/9/2024.
//

#include "PieceDescriptor.h"

std::ostream& operator<<(std::ostream& out, const PieceDescriptor& piece) {
    out << (piece.getSource() == SourceType::Add ? "Add, " : "Original, ")
        << piece.getStart() << ", " << piece.getLength();
    return out;
}

PieceDescriptor::PieceDescriptor(SourceType source, size_t start, size_t length)
    : m_source(source), m_start(start), m_length(length) {}

PieceDescriptor::PieceDescriptor(PieceDescriptor &piece)
    : m_source(piece.getSource()), m_start(piece.getStart()), m_length(piece.getLength()) {}

PieceDescriptor::PieceDescriptor(PieceDescriptor *piece)
    : m_source(piece->getSource()), m_start(piece->getStart()), m_length(piece->getLength())  {}

PieceDescriptor::~PieceDescriptor() {}

SourceType PieceDescriptor::getSource() const {
    return m_source;
}

size_t PieceDescriptor::getStart() const {
    return m_start;
}

size_t PieceDescriptor::getLength() const {
    return m_length;
}

void PieceDescriptor::setSource(SourceType mSource) {
    m_source = mSource;
}

void PieceDescriptor::setStart(size_t mStart) {
    m_start = mStart;
}

void PieceDescriptor::setLength(size_t mLength) {
    m_length = mLength;
}

std::pair<PieceDescriptor *, PieceDescriptor *> PieceDescriptor::splitPiece(PieceDescriptor *piece, size_t splitIndex) {
    if (splitIndex == 0 || splitIndex >= piece->getLength())
        return {nullptr, nullptr};

    auto leftPiece = new PieceDescriptor(piece->getSource(), piece->getStart(), splitIndex);
    auto rightPiece = new PieceDescriptor(piece->getSource(), piece->getStart() + splitIndex, piece->getLength() - splitIndex);

    return {leftPiece, rightPiece};
}

PieceDescriptor* PieceDescriptor::cutoffFromLeft(PieceDescriptor *piece, size_t offset) {
    if (offset >= piece->getLength() || offset == 0)
        return nullptr;

    auto cutoffPiece = new PieceDescriptor(piece->getSource(), piece->getStart(), offset);

    piece->setStart(piece->getStart() + offset);
    piece->setLength(piece->getLength() - offset);

    return cutoffPiece;
}

PieceDescriptor* PieceDescriptor::cutoffFromRight(PieceDescriptor *piece, size_t offset) {
    if (offset >= piece->getLength() || offset == 0)
        return nullptr;

    auto cutoffPiece = new PieceDescriptor(piece->getSource(), piece->getStart() + piece->getLength() - offset, offset);

    piece->setLength(piece->getLength() - offset);

    return cutoffPiece;
}


