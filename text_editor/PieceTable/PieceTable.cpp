//
// Created by bbard on 6/9/2024.
//

#include "PieceTable.h"

std::ostream& operator<<(std::ostream& out, const PieceTable& table) {
    out << "PIECES SIZE: " << table.m_pieces.size() << std::endl;

    for (auto piece : table.m_pieces) {
        out << *piece << std::endl;
    }

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

    for (auto piece : m_pieces)
        delete piece;

    while (!m_undoStack.empty()) {
        delete m_undoStack.top();
        m_undoStack.pop();
    }

    while (!m_redoStack.empty()) {
        delete m_redoStack.top();
        m_redoStack.pop();
    }
}

void PieceTable::insert(SourceType sourceType, size_t start, size_t length, size_t index, bool undoRedo) {
    if (length == 0) {
        std::cerr << "Returned" <<  std::endl;
        return;
    }

    if (index > m_size)
        index = m_size;

    PieceDescriptor* newPiece = new PieceDescriptor(sourceType, start, length);

    if (index == 0) {
        prepend(newPiece);
    } else if (index == m_size) {
        append(newPiece);
    } else {
        size_t currentIndex = 0;
        size_t i = 0;

        for (auto piece : m_pieces) {
            auto pieceLength = piece->getLength();

            if (index == currentIndex) {
                insertPiece(newPiece, i);
                break;
            } else if (index > currentIndex && index < currentIndex + pieceLength) {
                auto [leftPiece, rightPiece] = PieceDescriptor::splitPiece(piece, index - currentIndex);

                erasePiece(i);
                insertPiece(leftPiece, i);
                insertPiece(newPiece, i + 1);
                insertPiece(rightPiece, i+2);
                break;
            }

            ++i;
            currentIndex += pieceLength;
        }
    }

    addToUndo(new ActionDescriptor(ActionType::Insert, {new PieceDescriptor(newPiece)}, index), undoRedo);
    m_size += length;
}

void PieceTable::insert(std::string text, size_t index, bool undoRedo) {
    insert(SourceType::Add, m_addBuffer->size(), text.size(), index, undoRedo);
    insertTextInBuffer(text);
}


// Deletes text from range [start, end)
void PieceTable::deleteText(size_t start, size_t end, bool undoRedo) {
    if (start > end || start > m_size)
        return;

    if (end > m_size)
        end = m_size;

    size_t deleteLength = end - start;
    size_t currentIndex = 0;
    size_t i = 0;

    std::vector<PieceDescriptor*> pieceDescriptors;

    for (auto piece : m_pieces) {
        auto pieceStart = piece->getStart();
        auto pieceLength = piece->getLength();

        if (start == currentIndex && deleteLength == pieceLength) {
            pieceDescriptors.push_back(new PieceDescriptor(piece));
            erasePiece(i);

            break;
        } else if (start >= currentIndex && start < currentIndex + pieceLength && end < currentIndex + pieceLength) {
            PieceDescriptor* deletePiece = nullptr;

            if (start == currentIndex) {
                deletePiece = PieceDescriptor::cutoffFromLeft(piece, end - start);
            } else if (end == currentIndex + pieceLength - 1) {
                deletePiece = PieceDescriptor::cutoffFromRight(piece, end - start);
            } else {
                deletePiece = cutoffFromMiddle(piece, i, start - currentIndex, end - currentIndex);
            }

            pieceDescriptors.push_back(deletePiece);

            break;
        } else if (start >= currentIndex && start < currentIndex + pieceLength) {


            size_t newCurrentIndex = currentIndex + pieceLength;
            auto it = std::next(m_pieces.begin(), i);

            std::vector<decltype(it)> toDelete;

            auto deleteOffset = start - currentIndex;
            auto cutoffLength = pieceLength - deleteOffset;

            if (deleteOffset == 0) {
                pieceDescriptors.push_back(new PieceDescriptor(piece));
                toDelete.push_back(it);
            } else {
                auto cutoffPiece = PieceDescriptor::cutoffFromRight(piece, cutoffLength);
                pieceDescriptors.push_back(cutoffPiece);
            }

            it = std::next(it);

            while (it != m_pieces.end()) {
                auto itPiece = (*it);

                if (newCurrentIndex + itPiece->getLength() - 1 < end) {
                    toDelete.push_back(it);
                    pieceDescriptors.push_back(itPiece);
                } else {
                    auto offset = end - newCurrentIndex;

                    auto cutoffPiece = PieceDescriptor::cutoffFromLeft(itPiece, offset);

                    pieceDescriptors.push_back(cutoffPiece);

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

    addToUndo(new ActionDescriptor(ActionType::Delete, pieceDescriptors, start), undoRedo);
    m_size -= deleteLength;
}

void PieceTable::undo() {
    reverseOperation(m_undoStack, m_redoStack);
}

void PieceTable::redo() {
    reverseOperation(m_redoStack, m_undoStack);
}

size_t PieceTable::getSize() const { return m_size; }

void PieceTable::append(PieceDescriptor* piece) {
    m_pieces.push_back(piece);
}

void PieceTable::prepend(PieceDescriptor* piece) {
    m_pieces.push_front(piece);
}

inline void PieceTable::insertPiece(PieceDescriptor *piece, size_t index) {
        m_pieces.insert(std::next(m_pieces.begin(), index), piece);
}

inline void PieceTable::erasePiece(size_t index) {
    auto it = std::next(m_pieces.begin(), index);
    m_pieces.erase(it);
    delete *it;
}

PieceDescriptor* PieceTable::cutoffFromMiddle(PieceDescriptor *piece, size_t index, size_t leftOffset, size_t rightOffset) {
    if (leftOffset > piece->getLength() || rightOffset > piece->getLength())
        return nullptr;

    auto cutoffRightPiece = PieceDescriptor::cutoffFromRight(piece, piece->getLength() - leftOffset);
    auto cutoffMiddlePiece = PieceDescriptor::cutoffFromLeft(cutoffRightPiece, rightOffset - leftOffset);

    insertPiece(cutoffRightPiece, index+1);

    return  cutoffMiddlePiece;
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

    auto actionType = action->getActionType();
    auto descriptors = action->getDescriptors();
    auto index = action->getIndex();
    size_t totalLength = 0;

    if (actionType == ActionType::Insert) {
        std::cerr << "REVERSING INSERT" << std::endl;

        for (size_t i=0; i<descriptors.size(); ++i) {
            std::cerr << *descriptors[i] << std::endl;
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

    auto oppositeActionType = ActionDescriptor::getOppositeActionType(actionType);
    action->setActionType(oppositeActionType);

    reverseStack.push(action);
}


void PieceTable::addToUndo(ActionDescriptor *actionDescriptor, bool undoRedo) {
    if (!undoRedo) {
        //std::cerr << "ADDING TO UNDO: " << *actionDescriptor << std::endl;
        m_undoStack.push(actionDescriptor);
    }
}

