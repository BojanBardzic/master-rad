//
// Created by bbard on 6/9/2024.
//

#ifndef TEXT_EDITOR_PIECETABLE_H
#define TEXT_EDITOR_PIECETABLE_H

#include <numeric>
#include <iostream>
#include <string>
#include <list>
#include <fstream>
#include <stack>
#include "DeleteBuffer.h"
#include "InsertBuffer.h"
#include "PieceDescriptor.h"
#include "ActionDescriptor.h"

class PieceTable {
public:
    friend std::ostream& operator<<(std::ostream& out, const PieceTable& table);

    PieceTable();
    PieceTable(std::string& originalBuffer);
    ~PieceTable();

    bool insertChar(char c, size_t index);
    void insert(SourceType sourceType, size_t start, size_t length, size_t index, bool undoRedo = false);
    void insert(std::string text, size_t index, bool undoRedo = false);

    bool backspace(size_t index);
    bool charDelete(size_t index);
    void deleteText(size_t start, size_t end, bool undoRedo = false);

    void undo();
    void redo();

    void save(const std::string& filename);

    bool flushInsertBuffer();
    bool flushDeleteBuffer();

    void clearUndoAndRedoStacks();

    size_t getSize() const;
    bool isUndoEmpty() const;
    bool isRedoEmpty() const;
private:
    void append(PieceDescriptor* piece);
    void prepend(PieceDescriptor* piece);

    void insertPiece(PieceDescriptor* piece, size_t index);
    void erasePiece(size_t index);

    bool isPieceOnEndOffBuffer(PieceDescriptor* piece) const;
    static bool isInsidePiece(const size_t& index, const size_t& currentIndex, const size_t& length);
    static bool isInsidePieceInclusive(const size_t& index, const size_t& currentIndex, const size_t& length);

    PieceDescriptor* cutoffFromMiddle(PieceDescriptor* piece, size_t index, size_t leftOffset, size_t rightOffset);

    void reverseOperation(std::stack<ActionDescriptor*>& stack, std::stack<ActionDescriptor*>& reverseStack);

    void addToUndo(ActionDescriptor* actionDescriptor, bool undoRedo);
    void clearUndoStack();
    void clearRedoStack();

    void insertTextInBuffer(std::string& text);

    std::string* m_originalBuffer;
    std::string* m_addBuffer;
    InsertBuffer* m_insertBuffer;
    DeleteBuffer* m_deleteBuffer;
    std::list<PieceDescriptor*> m_pieces;
    std::stack<ActionDescriptor*> m_undoStack;
    std::stack<ActionDescriptor*> m_redoStack;
    size_t m_size;
};


#endif //TEXT_EDITOR_PIECETABLE_H
