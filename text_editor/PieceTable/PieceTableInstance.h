//
// Created by bbard on 7/20/2024.
//

#ifndef TEXT_EDITOR_PIECETABLEINSTANCE_H
#define TEXT_EDITOR_PIECETABLEINSTANCE_H

#include "PieceTable.h"

class PieceTableInstance {
public:
    PieceTableInstance();
    ~PieceTableInstance();

    void newFile();
    void open(std::string& filename);

    PieceTable& getInstance() const;
private:
    PieceTable* m_pieceTable{};
};


#endif //TEXT_EDITOR_PIECETABLEINSTANCE_H
