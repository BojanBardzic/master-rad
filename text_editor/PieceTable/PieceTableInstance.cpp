//
// Created by bbard on 7/20/2024.
//

#include "PieceTableInstance.h"

PieceTableInstance::PieceTableInstance() : m_file(nullptr) {
    m_pieceTable = new PieceTable();
}

PieceTableInstance::~PieceTableInstance() {
    delete m_pieceTable;
    delete m_file;
}

void PieceTableInstance::newFile() {
    auto oldFile = m_file;
    m_file = nullptr;
    delete oldFile;

    auto oldTable = m_pieceTable;
    m_pieceTable = new PieceTable();
    delete oldTable;
}

void PieceTableInstance::open(std::string &buffer, std::string& filePath) {
    // Create new instance for PieceTable and delete old One
    auto oldTable = m_pieceTable;
    m_pieceTable = new PieceTable(buffer);
    delete oldTable;

    // Update file information
    m_file = new File(filePath);
}

PieceTable& PieceTableInstance::getInstance() const { return *m_pieceTable; }

File* PieceTableInstance::getFile() const { return m_file; }

void PieceTableInstance::setFile(std::string &filePath) {
    if (m_file != nullptr) {
        auto oldFile = m_file;
        m_file = nullptr;
        delete oldFile;
    }

    m_file = new File(filePath);
}
