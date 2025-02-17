//
// Created by bbard on 7/20/2024.
//

#ifndef TEXT_EDITOR_PIECETABLEINSTANCE_H
#define TEXT_EDITOR_PIECETABLEINSTANCE_H

#include "PieceTable.h"
#include "../File.h"

class PieceTableInstance {
public:
    PieceTableInstance();
    ~PieceTableInstance();

    void newFile();
    void open(std::string& buffer, std::string& filePath);

    PieceTable& getInstance() const;
    File* getFile() const;

    void setFile(std::string& filePath);
private:
    PieceTable* m_pieceTable;
    File* m_file;
};


#endif //TEXT_EDITOR_PIECETABLEINSTANCE_H
