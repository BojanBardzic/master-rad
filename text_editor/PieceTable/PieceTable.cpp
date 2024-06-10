//
// Created by bbard on 6/9/2024.
//

#include "PieceTable.h"

std::ostream& operator<<(std::ostream& out, const PieceTable& table) {
    for (auto piece : table.m_pieces) {
        if (piece->getSource() == SourceType::Original) {
            out << table.m_originalBuffer->substr(piece->getStart(), piece->getLength());
        } else {
            out << table.m_addBuffer->substr(piece->getStart(), piece->getLength());
        }
    }

    return out;
}

PieceTable::PieceTable() : m_size(0) {
    m_originalBuffer = new std::string("");
    m_addBuffer = new std::string("");
}

PieceTable::PieceTable(std::string& filename) {
    m_originalBuffer = new std::string("");
    m_addBuffer = new std::string("");

    std::ifstream input(filename);

    std::getline(input, *m_originalBuffer, '\0');

    m_size = m_originalBuffer->size();

    input.close();
}

PieceTable::~PieceTable() {
    delete m_originalBuffer;
    delete m_addBuffer;
}

void PieceTable::insert(SourceType sourceType, size_t start, size_t length, size_t index) {
    if (length == 0) {
        return;
    }

    if (index == 0) {
        prepend(sourceType, start, length);
    } else if (index == m_size) {
        append(sourceType, start, length);
    } else {
        size_t currentIndex = 0;
        size_t i = 0;

        for (auto piece : m_pieces) {
            auto pieceStart = piece->getStart();
            auto pieceLength = piece->getLength();

            if (index == currentIndex) {
                auto descriptor = new PieceDescriptor(sourceType, start, length);
                insertPiece(descriptor, i);

                break;
            } else if (index > currentIndex && index < currentIndex + pieceLength) {
                size_t leftStart = pieceStart;
                size_t leftLength = index - currentIndex;

                size_t rightStart = leftStart + leftLength;
                size_t rightLength = pieceLength - leftLength;

                auto pieceSourceType = piece->getSource();

                auto leftPieceDescriptor = new PieceDescriptor(pieceSourceType, leftStart, leftLength);
                auto newDescriptor = new PieceDescriptor(sourceType, start, length);
                auto rightPieceDescriptor = new PieceDescriptor(pieceSourceType, rightStart, rightLength);

                erasePiece(i);

                insertPiece(leftPieceDescriptor, i);
                insertPiece(newDescriptor, i+1);
                insertPiece(rightPieceDescriptor, i+2);

                break;
            }

            ++i;
            currentIndex += pieceLength;
        }
    }

    m_size += length;
}

void PieceTable::insert(std::string text, size_t index) {
    insert(SourceType::Add, m_addBuffer->size(), text.size(), index);
    insertTextInBuffer(text);
}

void PieceTable::deleteText(size_t start, size_t end) {
    if (start > end || start > m_size)
        return;

    if (end > m_size)
        end = m_size;

    size_t deleteLength = end - start;
    size_t currentIndex = 0;
    size_t i = 0;

    for (auto piece : m_pieces) {
        auto pieceStart = piece->getStart();
        auto pieceLength = piece->getLength();

        if (start == currentIndex && deleteLength == pieceLength) {
            erasePiece(i);
            break;
        } else if (start >= currentIndex && start < currentIndex + pieceLength && end < currentIndex + pieceLength) {
            auto sourceType = piece->getSource();

            size_t leftStart = pieceStart;
            size_t leftLength = start - currentIndex;

            size_t rightStart = pieceStart + (end - currentIndex);
            size_t rightLength = pieceLength - (end - currentIndex);

            size_t deleteStart = leftStart + leftLength;
            size_t deleteLength = pieceLength - leftLength - rightLength;

            auto deletePiece = new PieceDescriptor(sourceType, deleteStart, deleteLength);

            auto leftPiece = new PieceDescriptor(sourceType, leftStart, leftLength);
            auto rightPiece = new PieceDescriptor(sourceType, rightStart, rightLength);

            erasePiece(i);

            insertPiece(leftPiece, i);
            insertPiece(rightPiece, i+1);

            break;
        } else if (start >= currentIndex && start < currentIndex + pieceLength) {
            std::vector<PieceDescriptor*> pieceDescriptors;

            size_t newCurrentIndex = currentIndex + pieceLength;
            auto it = std::next(m_pieces.begin(), i+1);

            std::vector<decltype(it)> toDelete;

            auto deleteOffset = start - currentIndex;
            auto cutoffLength = pieceLength - deleteOffset;

            pieceDescriptors.push_back(
                    new PieceDescriptor(piece->getSource(), pieceStart + deleteOffset, pieceLength - deleteOffset)
                    );

            cutoffFromRight(piece, cutoffLength);

            while (it != m_pieces.end()) {
                auto itPiece = (*it);

                if (newCurrentIndex + itPiece->getLength() - 1 < end) {
                    toDelete.push_back(it);
                    pieceDescriptors.push_back(itPiece);
                } else {
                    auto offset = end - newCurrentIndex;

                    pieceDescriptors.push_back(
                            new PieceDescriptor(itPiece->getSource(), itPiece->getStart(), offset)
                            );

                    cutoffFromLeft(itPiece, offset);

                    break;
                }

                newCurrentIndex += itPiece->getLength();
                it = std::next(it);
            }

            for (size_t i=0; i<toDelete.size(); ++i) {
                m_pieces.erase(toDelete[i]);
            }

            break;
        }


        ++i;
        currentIndex += piece->getLength();
    }

    m_size -= deleteLength;
}

size_t PieceTable::getSize() const { return m_size; }

void PieceTable::append(SourceType sourceType, size_t start, size_t length) {
    auto addDescriptor = new PieceDescriptor(sourceType, start, length);
    m_pieces.push_back(addDescriptor);
}

void PieceTable::prepend(SourceType sourceType, size_t start, size_t length) {
    auto addDescriptor = new PieceDescriptor(sourceType, start, length);
    m_pieces.push_front(addDescriptor);
}

inline void PieceTable::insertPiece(PieceDescriptor *piece, size_t index) {
    m_pieces.insert(std::next(m_pieces.begin(), index), piece);
}

inline void PieceTable::erasePiece(size_t index) {
    m_pieces.erase(std::next(m_pieces.begin(), index));
}

inline void PieceTable::cutoffFromRight(PieceDescriptor *piece, size_t offset) {
    piece->setLength(piece->getLength() - offset);
}

inline void PieceTable::cutoffFromLeft(PieceDescriptor *piece, size_t offset) {
    piece->setStart(piece->getStart() + offset);
    piece->setLength(piece->getLength() - offset);
}

inline void PieceTable::insertTextInBuffer(std::string &text) {
    *m_addBuffer += text;
}

