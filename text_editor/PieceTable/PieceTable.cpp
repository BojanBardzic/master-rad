//
// Created by bbard on 6/9/2024.
//

#include "PieceTable.h"

std::ostream& operator<<(std::ostream& out, const PieceTable& table) {
    bool shouldPrintInsertBuffer = !table.m_insertBuffer->isFlushed();
    bool shouldPrintDeleteBuffer = !table.m_deleteBuffer->isFlushed();

    size_t current_index = 0;

    if (table.m_pieces.empty() && !table.m_insertBuffer->isFlushed()) {
        out << table.m_insertBuffer->getContent();
        shouldPrintInsertBuffer = false;
    }

    auto pieceIt = table.m_pieces.begin();
    while (pieceIt != table.m_pieces.end()) {
        auto piece = *pieceIt;

        std::string* buffer = piece->getSource() == SourceType::Original ? table.m_originalBuffer : table.m_addBuffer;

        if (shouldPrintInsertBuffer && PieceTable::isInsidePiece(table.m_insertBuffer->getStartIndex(), current_index, piece->getLength())) {
            auto leftStringLen = table.m_insertBuffer->getStartIndex() - current_index;

            out << buffer->substr(piece->getStart(), leftStringLen);
            out << table.m_insertBuffer->getContent();
            out << buffer->substr(piece->getStart() + leftStringLen, piece->getLength() - leftStringLen);

            shouldPrintInsertBuffer = false;
        } else if (shouldPrintDeleteBuffer && PieceTable::isInsidePiece(table.m_deleteBuffer->getStartIndex(), current_index, piece->getLength())) {
            std::cerr << "Printing delete buffer" << std::endl;
            std::cerr << "Piece" << *piece << std::endl;
            std::cerr << "Current index: " << current_index << std::endl;
            auto deleteBufferStart = table.m_deleteBuffer->getStartIndex();
            auto deleteBufferEnd = table.m_deleteBuffer->getEndIndex();

            std::cerr << "start: " << deleteBufferStart << std::endl;
            std::cerr << "end: " << deleteBufferEnd << std::endl;

            if (deleteBufferStart == current_index && deleteBufferEnd == current_index + piece->getLength()) {
                std::cerr << "Doing nothing" << std::endl;
                // Do nothing
            } else if (deleteBufferStart >= current_index && deleteBufferEnd <= current_index + piece->getLength()) {
                std::cerr << "Deleting in-between" << std::endl;

                auto leftStringLen = deleteBufferStart - current_index;
                auto deleteLen = deleteBufferEnd - deleteBufferStart;
                auto rightStringLen = piece->getLength() - deleteLen - leftStringLen;
                auto rightOffset = std::min(leftStringLen + deleteLen, piece->getLength()-1);

                out << buffer->substr(piece->getStart(), leftStringLen);
                out << buffer->substr(piece->getStart() + rightOffset, rightStringLen);
            } else {
                std::cerr << "Deleting across multiple pieces" << std::endl;

                auto leftLen = table.m_deleteBuffer->getStartIndex() - current_index;
                std::cerr << "left len: " << leftLen << std::endl;

                out << buffer->substr(piece->getStart(), leftLen);


                while (pieceIt != table.m_pieces.end() && !PieceTable::isInsidePieceInclusive(table.m_deleteBuffer->getEndIndex(), current_index, piece->getLength())) {
                    std::cerr << "Iterating over piece" << std::endl;
                    current_index += piece->getLength();
                    pieceIt = std::next(pieceIt);
                    piece = pieceIt == table.m_pieces.end() ? nullptr : *pieceIt;
                }

                if (pieceIt == table.m_pieces.end()) {
                    std::cerr << "Reached end, breaking out of loop" << std::endl;
                    break;
                }

                piece = *pieceIt;
                std::cerr << "Piece: " << *piece << std::endl;

                auto rightOffset = table.m_deleteBuffer->getEndIndex() - current_index;
                std::cerr << "right offset: " << rightOffset << std::endl;

                out << buffer->substr(piece->getStart() + rightOffset, piece->getLength() - rightOffset);
            }

            shouldPrintDeleteBuffer = false;
        } else {
            std::cerr << "Printing whole: " << buffer->substr(piece->getStart(), piece->getLength()) << std::endl;
            out << buffer->substr(piece->getStart(), piece->getLength());
        }

        current_index += piece->getLength();
        pieceIt = std::next(pieceIt);
    }

    if (shouldPrintInsertBuffer) {
        out << table.m_insertBuffer->getContent();
    }

    std::cerr << "EXITED PRINTING" << std::endl;

    return out;
}

PieceTable::PieceTable() : m_size(0) {
    m_originalBuffer = new std::string("");
    m_addBuffer = new std::string("");
    m_insertBuffer = new InsertBuffer();
    m_deleteBuffer = new DeleteBuffer();
}

PieceTable::PieceTable(std::string& originalBuffer) : m_size(0) {
    m_originalBuffer = new std::string(originalBuffer);
    m_addBuffer = new std::string("");
    m_insertBuffer = new InsertBuffer();
    m_deleteBuffer = new DeleteBuffer();

    insert(SourceType::Original, 0, m_originalBuffer->size(), 0, true);
}

PieceTable::~PieceTable() {
    delete m_originalBuffer;
    delete m_addBuffer;
    delete m_insertBuffer;
    delete m_deleteBuffer;

    for (auto piece : m_pieces)
        delete piece;

    clearUndoStack();
    clearRedoStack();
}


// Inserts char to add buffer, returns if the buffer has been initialized
bool PieceTable::insertChar(char c, size_t index) {
    if (index != m_insertBuffer->getEndIndex()) {
        flushInsertBuffer();
    }

    bool result = false;

    if (m_insertBuffer->isFlushed()) {
        m_insertBuffer->setStartIndex(index);
        m_insertBuffer->setEndIndex(index);
        m_insertBuffer->setFlushed(false);
        result = true;
    }

    m_insertBuffer->appendToContent(c);
    return  result;
}

// Inserts text from the addBuffer into the pieceTable
void PieceTable::insert(SourceType sourceType, size_t start, size_t length, size_t index, bool undoRedo) {
    if (length == 0) {
        return;
    }

    if (index > m_size)
        index = m_size;

    auto newPiece = new PieceDescriptor(sourceType, start, length);


    if (index == 0) {
        prepend(newPiece);
    } else if (index == m_size) {
        auto endPiece = m_pieces.back();

        // If the piece is on the end of the add buffer we just extend the current piece
        if (isPieceOnEndOffBuffer(endPiece)) {
            endPiece->setLength(endPiece->getLength() + length);
        } else {
            std::cerr << "Appending" << std::endl;
            append(newPiece);
        }
    } else {
        // Else we iterate through the pieces to find the correct index
        size_t currentIndex = 0;
        size_t i = 0;
        PieceDescriptor* previousPiece = nullptr;

        for (auto piece : m_pieces) {
            auto pieceLength = piece->getLength();

            // If the index is exactly between two pieces then we insert a new piece between them
            if (index == currentIndex) {
                // If the piece is on the end of the buffer we append to the previous piece
                if (isPieceOnEndOffBuffer(previousPiece)) {
                    previousPiece->setLength(previousPiece->getLength() + length);
                } else {
                    insertPiece(newPiece, i);
                }

                break;
            } else if (index > currentIndex && index < currentIndex + pieceLength) {
                // If the inserting index is inside a piece we split it into two then add the new piece between them
                auto [leftPiece, rightPiece] = PieceDescriptor::splitPiece(piece, index - currentIndex);


                erasePiece(i);

                insertPiece(leftPiece, i);

                insertPiece(newPiece, i+1);

                insertPiece(rightPiece, i+2);
                break;
            }

            ++i;
            currentIndex += pieceLength;
            previousPiece = piece;
        }
    }

    addToUndo(new ActionDescriptor(ActionType::Insert, {new PieceDescriptor(newPiece)}, index), undoRedo);
    m_size += length;
}

void PieceTable::insert(std::string text, size_t index, bool undoRedo) {
    insert(SourceType::Add, m_addBuffer->size(), text.size(), index, undoRedo);
    insertTextInBuffer(text);
}

bool PieceTable::backspace(size_t index) {
    std::cerr << "Entered backspace" << std::endl;
    if (index != m_deleteBuffer->getDeleteIndex()) {
        flushDeleteBuffer();
    }

    bool result = false;

    if (m_deleteBuffer->isFlushed()) {
        m_deleteBuffer->setStartIndex(index);
        m_deleteBuffer->setEndIndex(index);
        m_deleteBuffer->setDeleteIndex(index);
        m_deleteBuffer->setFlushed(false);
        result = true;
    }

    if (m_deleteBuffer->getStartIndex() != 0)
        m_deleteBuffer->setStartIndex(m_deleteBuffer->getStartIndex()-1);
    if (index != 0)
        m_deleteBuffer->setDeleteIndex(index-1);

    return result;
}

bool PieceTable::charDelete(size_t index) {
    std::cerr << "ENTERED CHAR DELETE!" << std::endl;

    if (index != m_deleteBuffer->getDeleteIndex()) {
        flushDeleteBuffer();
    }

    bool result = false;

    if (m_deleteBuffer->isFlushed()) {
        m_deleteBuffer->setDeleteIndex(index);
        m_deleteBuffer->setStartIndex(index);
        m_deleteBuffer->setEndIndex(index);
        m_deleteBuffer->setFlushed(false);
        result = true;
    }

    auto newIndex = std::min(m_deleteBuffer->getEndIndex()+1, getSize());
    std::cerr << "END INDEX: " << m_deleteBuffer->getEndIndex() << std::endl;
    std::cerr << "SIZE: " << getSize() << std::endl;
    std::cerr << "NEW INDEX: " << newIndex << std::endl;
    m_deleteBuffer->setEndIndex(newIndex);

    return result;
}

bool PieceTable::addTabs(const std::vector<size_t> &indices) {
    for (const size_t index : indices) {
        insert("\t", index);
    }

    return !indices.empty();
}

bool PieceTable::removeTabs(const std::vector<size_t> &indices) {
    for (const size_t index : indices) {
        deleteText(index, index+1);
    }

    return !indices.empty();
}


// Deletes text from range [start, end)
void PieceTable::deleteText(size_t start, size_t end, bool undoRedo) {
    if (start >= end || start >= m_size || m_pieces.empty())
        return;

    if (end > m_size)
        end = m_size;

    size_t deleteLength = end - start;
    size_t currentIndex = 0;
    size_t i = 0;

    std::vector<PieceDescriptor*> pieceDescriptors;

    for (auto piece : m_pieces) {
        auto pieceLength = piece->getLength();

        if (start == currentIndex && deleteLength == pieceLength) {
            pieceDescriptors.push_back(new PieceDescriptor(piece));
            erasePiece(i);

            break;
        } else if (start >= currentIndex && start < currentIndex + pieceLength && end <= currentIndex + pieceLength) {
            PieceDescriptor* deletePiece;

            if (start == currentIndex) {
                deletePiece = PieceDescriptor::cutoffFromLeft(piece, deleteLength);
            } else if (end == currentIndex + pieceLength) {
                deletePiece = PieceDescriptor::cutoffFromRight(piece, deleteLength);
            } else {
                deletePiece = cutoffFromMiddle(piece, i, start - currentIndex, pieceLength - (end - currentIndex));
            }

            if (deletePiece != nullptr)
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

                if (cutoffPiece != nullptr)
                    pieceDescriptors.push_back(cutoffPiece);
            }

            it = std::next(it);

            while (it != m_pieces.end()) {
                auto itPiece = (*it);

                if (newCurrentIndex + itPiece->getLength() - 1 < end) {
                    toDelete.push_back(it);
                    pieceDescriptors.push_back(new PieceDescriptor(itPiece));
                } else {
                    auto offset = end - newCurrentIndex;

                    auto cutoffPiece = PieceDescriptor::cutoffFromLeft(itPiece, offset);

                    if (cutoffPiece != nullptr)
                        pieceDescriptors.push_back(cutoffPiece);

                    break;
                }

                newCurrentIndex += itPiece->getLength();
                it = std::next(it);
            }

            for (size_t j=0; j<toDelete.size(); ++j) {
                m_pieces.erase(toDelete[j]);
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

void PieceTable::save(const std::string &filename) {

}

bool PieceTable::flushInsertBuffer() {
    if (!m_insertBuffer->isFlushed()) {
        std::cerr << "Flushing buffer with index: " << m_insertBuffer->getStartIndex() << std::endl;
        std::cerr << "Table Size: " << m_size << std::endl;
        std::cerr << "Content: " << m_insertBuffer->getContent() << std::endl;
        insert(m_insertBuffer->getContent(), m_insertBuffer->getStartIndex());
        m_insertBuffer->clearContent();
        m_insertBuffer->setFlushed(true);
        return true;
    }

    return false;
}

bool PieceTable::flushDeleteBuffer() {
    if (!m_deleteBuffer->isFlushed()) {
        std::cerr << "Flushing delete buffer" << std::endl;
        std::cerr << "Flushing delete from " << m_deleteBuffer->getStartIndex() << " to " << m_deleteBuffer->getEndIndex() << std::endl;
        deleteText(m_deleteBuffer->getStartIndex(), m_deleteBuffer->getEndIndex());
        m_deleteBuffer->setFlushed(true);
        return true;
    }

    return false;
}

void PieceTable::clearUndoAndRedoStacks() {
    clearUndoStack();
    clearRedoStack();
}

size_t PieceTable::getSize() const { return m_size; }

bool PieceTable::isUndoEmpty() const { return m_undoStack.empty(); }

bool PieceTable::isRedoEmpty() const { return m_redoStack.empty(); }

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
    auto ptr = *it;

    m_pieces.erase(it);
    delete ptr;
}

bool PieceTable::isPieceOnEndOffBuffer(PieceDescriptor *piece) const {
    return piece->getSource() == SourceType::Add && piece->getStart() + piece->getLength() == m_addBuffer->size();
}

bool PieceTable::isInsidePiece(const size_t &index, const size_t &currentIndex, const size_t &length) {
    return index >= currentIndex && index < currentIndex + length;
}

bool PieceTable::isInsidePieceInclusive(const size_t& index, const size_t& currentIndex, const size_t& length) {
    return index >= currentIndex && index <= currentIndex + length;
}

PieceDescriptor* PieceTable::cutoffFromMiddle(PieceDescriptor *piece, size_t index, size_t leftOffset, size_t rightOffset) {
    if (leftOffset > piece->getLength() || rightOffset > piece->getLength())
        return nullptr;

    auto length = piece->getLength();

    auto cutoffRightPiece = PieceDescriptor::cutoffFromRight(piece, length - leftOffset);
    auto cutoffMiddlePiece = PieceDescriptor::cutoffFromLeft(cutoffRightPiece, length - rightOffset - leftOffset);

    insertPiece(cutoffRightPiece, index+1);

    return  cutoffMiddlePiece;
}

inline void PieceTable::insertTextInBuffer(std::string &text) {
    *m_addBuffer += text;
}

void PieceTable::reverseOperation(std::stack<ActionDescriptor *> &stack, std::stack<ActionDescriptor *> &reverseStack) {
    std::cerr << "ENTERED REVERSE OPERATION" << std::endl;

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

    std::cerr << "Descriptors.size() = " << descriptors.size() << std::endl;

    if (actionType == ActionType::Insert) {
        std::cerr << "REVERSING INSERT" << std::endl;

        totalLength = std::accumulate(descriptors.begin(), descriptors.end(), (size_t)0,
                                      [](size_t acc, PieceDescriptor* descriptor) { return  acc + descriptor->getLength(); }
                                      );


        std::cerr << "Exited for loop" << std::endl;

        deleteText(index, index+totalLength, true);
    } else {
        std::cerr << "REVERSING DELETE" << std::endl;

        for (size_t i=0; i<descriptors.size(); ++i) {
            std::cerr << "Entering iteration" << std::endl;
            auto source = descriptors[i]->getSource();
            auto start = descriptors[i]->getStart();
            auto length = descriptors[i]->getLength();

            std::cerr << "Starting " << i << "th insert" << std::endl;
            insert(source, start, length, index + totalLength, true);

            totalLength += length;
        }

        std::cerr << "Finishing reversing delete" << std::endl;
    }

    auto oppositeActionType = ActionDescriptor::getOppositeActionType(actionType);
    action->setActionType(oppositeActionType);

    reverseStack.push(action);
}


void PieceTable::addToUndo(ActionDescriptor *actionDescriptor, bool undoRedo) {
    if (!undoRedo) {
        m_undoStack.push(actionDescriptor);
    }
}

void PieceTable::clearUndoStack() {
    while (!m_undoStack.empty()) {
        m_undoStack.pop();
    }
}

void PieceTable::clearRedoStack() {
    while (!m_redoStack.empty()) {
        m_redoStack.pop();
    }
}





