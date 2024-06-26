//
// Created by bbard on 6/9/2024.
//

#ifndef TEXT_EDITOR_PIECETABLE_H
#define TEXT_EDITOR_PIECETABLE_H

#include <iostream>
#include <string>
#include <list>
#include <fstream>
#include <stack>
#include "PieceDescriptor.h"
#include "ActionDescriptor.h"

class PieceTable {
public:
    friend std::ostream& operator<<(std::ostream& out, const PieceTable& table);

    PieceTable();
    PieceTable(std::string& filename);
    ~PieceTable();

    void insert(SourceType sourceType, size_t start, size_t length, size_t index, bool undoRedo = false);
    void insert(std::string text, size_t index, bool undoRedo = false);

    void deleteText(size_t start, size_t end, bool undoRedo = false);

    void undo();
    void redo();

    void save(std::string& filename);

    size_t getSize() const;
private:
    void append(PieceDescriptor* piece);
    void prepend(PieceDescriptor* piece);

    void insertPiece(PieceDescriptor* piece, size_t index);
    void erasePiece(size_t index);

    PieceDescriptor* cutoffFromMiddle(PieceDescriptor* piece, size_t index, size_t leftOffset, size_t rightOffset);

    void reverseOperation(std::stack<ActionDescriptor*>& stack, std::stack<ActionDescriptor*>& reverseStack);

    void addToUndo(ActionDescriptor* actionDescriptor, bool undoRedo);

    void insertTextInBuffer(std::string& text);

    std::string* m_originalBuffer;
    std::string* m_addBuffer;
    std::list<PieceDescriptor*> m_pieces;
    std::stack<ActionDescriptor*> m_undoStack;
    std::stack<ActionDescriptor*> m_redoStack;
    size_t m_size;
};


#endif //TEXT_EDITOR_PIECETABLE_H
