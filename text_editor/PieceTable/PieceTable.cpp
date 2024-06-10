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

void PieceTable::insert(SourceType sourceType, size_t start, size_t length, size_t index, bool undoRedo) {
    if (length == 0) {
        std::cerr << "Returned" <<  std::endl;
        return;
    }

    if (index > m_size)
        index = m_size;

    if (index == 0) {
        prepend(sourceType, start, length, undoRedo);
    } else if (index == m_size) {
        append(sourceType, start, length, undoRedo);
    } else {
        size_t currentIndex = 0;
        size_t i = 0;

        for (auto piece : m_pieces) {
            auto pieceStart = piece->getStart();
            auto pieceLength = piece->getLength();

            if (index == currentIndex) {
                auto descriptor = new PieceDescriptor(sourceType, start, length);
                insertPiece(descriptor, i);

                addToUndo(new ActionDescriptor(ActionType::Insert, {descriptor}, index), undoRedo);

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

                addToUndo(new ActionDescriptor(ActionType::Insert, {newDescriptor}, index), undoRedo);

                break;
            }

            ++i;
            currentIndex += pieceLength;
        }
    }

    m_size += length;
}

void PieceTable::insert(std::string text, size_t index, bool undoRedo) {
    insert(SourceType::Add, m_addBuffer->size(), text.size(), index, undoRedo);
    insertTextInBuffer(text);
}

void PieceTable::deleteText(size_t start, size_t end, bool undoRedo) {
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
            addToUndo(new ActionDescriptor(ActionType::Delete, {piece}, start), undoRedo);

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

            addToUndo(new ActionDescriptor(ActionType::Delete, {deletePiece}, start), undoRedo);

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

            addToUndo(new ActionDescriptor(ActionType::Delete, pieceDescriptors, start), undoRedo);

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

void PieceTable::undo() {
    reverseOperation(m_undoStack, m_redoStack);
}

void PieceTable::redo() {
    reverseOperation(m_redoStack, m_undoStack);
}

size_t PieceTable::getSize() const { return m_size; }

void PieceTable::append(SourceType sourceType, size_t start, size_t length, bool undoRedo) {
    auto addDescriptor = new PieceDescriptor(sourceType, start, length);
    m_pieces.push_back(addDescriptor);
    addToUndo(new ActionDescriptor(ActionType::Insert, {addDescriptor}, m_size-1), undoRedo);
}

void PieceTable::prepend(SourceType sourceType, size_t start, size_t length, bool undoRedo) {
    auto addDescriptor = new PieceDescriptor(sourceType, start, length);
    m_pieces.push_front(addDescriptor);
    addToUndo(new ActionDescriptor(ActionType::Insert, {addDescriptor}, 0), undoRedo);
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

void PieceTable::reverseOperation(std::stack<ActionDescriptor *> &stack, std::stack<ActionDescriptor *> &reverseStack) {


    if (stack.empty()) {
        std::cerr << "RETURNED" << std::endl;
        return;
    }

    auto action = stack.top();
    stack.pop();

    auto descriptors = action->getDescriptors();
    auto index = action->getIndex();
    size_t totalLength = 0;

    if (action->getActionType() == ActionType::Insert) {
        std::cerr << "REVERSING INSERT" << std::endl;

        for (size_t i=0; i<descriptors.size(); ++i) {
            totalLength += descriptors[i]->getLength();
        }

        deleteText(index, index+totalLength, true);
    } else {
        std::cerr << "REVERSING DELETE" << std::endl;

        for (size_t i=0; i<descriptors.size(); ++i) {
            auto source = descriptors[i]->getSource();
            auto start = descriptors[i]->getStart();
            auto length = descriptors[i]->getLength();

            insert(source, start, length, index + totalLength, true);

            totalLength += length;
        }
    }

    auto oppositeActionType = ActionDescriptor::getOppositeActionType(action->getActionType());
    action->setActionType(oppositeActionType);

    reverseStack.push(action);
}

void PieceTable::splitPiece(size_t pieceIndex, size_t splitIndex) {
    if (pieceIndex >= m_size)
        return;

    auto it = std::next(m_pieces.begin(), pieceIndex);
    auto piece = *it;

    auto rightStart = piece->getStart() + splitIndex;
    auto rightLegnth = piece->getLength() - splitIndex;

    cutoffFromRight(piece, splitIndex);

    auto rightPiece = new PieceDescriptor(piece->getSource(), rightStart, rightLegnth);
    insertPiece(rightPiece, pieceIndex+1);
}

void PieceTable::addToUndo(ActionDescriptor *actionDescriptor, bool undoRedo) {
    if (!undoRedo)
        m_undoStack.push(actionDescriptor);
}

