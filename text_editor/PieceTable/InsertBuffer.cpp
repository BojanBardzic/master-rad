//
// Created by bbard on 7/11/2024.
//

#include "InsertBuffer.h"

InsertBuffer::InsertBuffer() : m_flushed(true), m_startIndex(0), m_endIndex(0) {}

InsertBuffer::~InsertBuffer() {}

const size_t &InsertBuffer::getStartIndex() const { return m_startIndex; }

const size_t &InsertBuffer::getEndIndex() const { return m_endIndex; }

bool InsertBuffer::isFlushed() const { return m_flushed; }

const std::string& InsertBuffer::getContent() const { return m_content; }

void InsertBuffer::setStartIndex(const size_t &startIndex) { m_startIndex = startIndex; }

void InsertBuffer::setEndIndex(const size_t &endIndex) { m_endIndex = endIndex; }

void InsertBuffer::setFlushed(bool flushed) { m_flushed = flushed; }

void InsertBuffer::appendToContent(char c) {
    m_content += c;
    m_endIndex += 1;
}

void InsertBuffer::clearContent() { m_content.clear(); }
