//
// Created by bbard on 9/9/2024.
//

#include "CodeBlock.h"

std::ostream& operator<<(std::ostream& out, const CodeBlock& block) {
    out << "start: " << block.m_start << ", "
        << "end: " << block.m_end << ", "
        << "folded: " << (block.m_folded ? "true" : "false");
    return out;
}

bool CodeBlock::operator==(const CodeBlock &other) const {
    return m_start == other.m_start && m_end == other.m_end;
}

bool CodeBlock::operator!=(const CodeBlock &other) const {
    return m_start != other.m_start || m_end != other.m_end;
}

bool CodeBlock::operator<(const CodeBlock &other) const {
    return m_start < other.m_start || (m_start == other.m_start && m_end < other.m_end);
}

bool CodeBlock::operator>(const CodeBlock &other) const {
    return m_start > other.m_start || (m_start == other.m_start && m_end > other.m_end);
}

CodeBlock::CodeBlock(TextCoordinates &start, TextCoordinates &end) : m_start(start), m_end(end), m_folded(false) {}

CodeBlock::CodeBlock(const CodeBlock& block) : m_start(block.m_start), m_end(block.m_end), m_folded(block.m_folded) {}

CodeBlock::~CodeBlock() {}

const TextCoordinates &CodeBlock::getStart() const { return m_start; }

const TextCoordinates &CodeBlock::getEnd() const { return m_end; }

const bool CodeBlock::isFolded() const { return m_folded; }

void CodeBlock::setStart(const TextCoordinates &start) { m_start = start; }

void CodeBlock::setEnd(const TextCoordinates &end) { m_end = end; }

void CodeBlock::setFolded(const bool folded) { m_folded = folded; }




