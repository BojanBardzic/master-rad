//
// Created by bbard on 7/12/2024.
//

#include "DeleteBuffer.h"

DeleteBuffer::DeleteBuffer() : m_flushed(true), m_startIndex(0), m_endIndex(0), m_deleteIndex(0) {}

DeleteBuffer::~DeleteBuffer() {}

const size_t &DeleteBuffer::getStartIndex() const { return m_startIndex; }

const size_t &DeleteBuffer::getEndIndex() const { return m_endIndex; }

const size_t &DeleteBuffer::getDeleteIndex() const { return m_deleteIndex; }

const size_t DeleteBuffer::getDeleteSize() const { return m_endIndex - m_startIndex; }

bool DeleteBuffer::isFlushed() const { return m_flushed; }

void DeleteBuffer::setStartIndex(const size_t &startIndex) { m_startIndex = startIndex; }

void DeleteBuffer::setEndIndex(const size_t &endIndex) { m_endIndex = endIndex; }

void DeleteBuffer::setDeleteIndex(const size_t &deleteIndex) { m_deleteIndex = deleteIndex; }

void DeleteBuffer::setFlushed(bool flushed) { m_flushed = flushed; }


