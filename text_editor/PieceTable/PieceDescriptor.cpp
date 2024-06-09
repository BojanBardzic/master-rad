//
// Created by bbard on 6/9/2024.
//

#include "PieceDescriptor.h"

PieceDescriptor::PieceDescriptor(SourceType source, size_t start, size_t length)
    : m_source(source), m_start(start), m_length(length) {}

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
