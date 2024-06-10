//
// Created by bbard on 6/9/2024.
//

#ifndef TEXT_EDITOR_PIECETABLE_H
#define TEXT_EDITOR_PIECETABLE_H

#include <string>
#include <list>
#include <fstream>
#include "PieceDescriptor.h"

class PieceTable {
public:
    friend std::ostream& operator<<(std::ostream& out, const PieceTable& table);

    PieceTable();
    PieceTable(std::string& filename);
    ~PieceTable();

    void insert(SourceType sourceType, size_t start, size_t length, size_t index);
    void insert(std::string text, size_t index);

    void deleteText(size_t start, size_t end);

    void save(std::string& filename);

    size_t getSize() const;
private:
    void append(SourceType sourceType, size_t start, size_t length);
    void prepend(SourceType sourceType, size_t start, size_t length);

    void insertPiece(PieceDescriptor* piece, size_t index);
    void erasePiece(size_t index);

    void cutoffFromRight(PieceDescriptor* piece, size_t offset);
    void cutoffFromLeft(PieceDescriptor* piece, size_t offset);

    void insertTextInBuffer(std::string& text);

    std::string* m_originalBuffer;
    std::string* m_addBuffer;
    std::list<PieceDescriptor*> m_pieces;
    size_t m_size;
};


#endif //TEXT_EDITOR_PIECETABLE_H
