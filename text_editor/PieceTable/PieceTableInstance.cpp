//
// Created by bbard on 7/20/2024.
//

#include "PieceTableInstance.h"

PieceTableInstance::PieceTableInstance() {
    m_pieceTable = new PieceTable();
}

PieceTableInstance::~PieceTableInstance() {
    delete m_pieceTable;
}

void PieceTableInstance::newFile() {
    auto oldTable = m_pieceTable;
    m_pieceTable = new PieceTable();
    delete oldTable;
}

void PieceTableInstance::open(std::string &filename) {
    auto oldTable = m_pieceTable;
    m_pieceTable = new PieceTable(filename);
    delete oldTable;
}

PieceTable& PieceTableInstance::getInstance() const { return *m_pieceTable; }
