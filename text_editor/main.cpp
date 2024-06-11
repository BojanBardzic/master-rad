#include <iostream>
#include "PieceTable/PieceTable.h"

void printTable(PieceTable* table) {
    std::cout << *table << std::endl;
}

void undoAndPrint(PieceTable* table, int n = 1) {
    for (int i=0; i<n; ++i) {
        table->undo();
        printTable(table);
    }
}

void redoAndPrint(PieceTable* table, int n = 1) {
    for (int i=0; i<n; ++i) {
        table->redo();
        printTable(table);
    }
}

void insertAndPrint(PieceTable* table, std::string text, size_t index) {
    table->insert(text, index);
    printTable(table);
}

void deleteAndPrint(PieceTable* table, size_t start, size_t end) {
    table->deleteText(start, end);
    printTable(table);
}

int main() {

//    PieceDescriptor* piece = new PieceDescriptor(SourceType::Add, 0, 10);
//    auto cutoffPiece = PieceDescriptor::cutoffFromRight(piece, 4);
//
//    if (piece != nullptr)
//        std::cout << *piece << std::endl;
//    if (cutoffPiece != nullptr)
//        std::cout << *cutoffPiece << std::endl;

    PieceTable* table = new PieceTable();

    insertAndPrint(table, "Something about text", 0);

    deleteAndPrint(table, 0, 5);

    deleteAndPrint(table, 7, 10);

    insertAndPrint(table, "Hello", 5);

    deleteAndPrint(table, 0, table->getSize());

    insertAndPrint(table, "Something new", 0);

    undoAndPrint(table, 6);
    redoAndPrint(table, 6);

    delete table;

    return 0;
}
