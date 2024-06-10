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

int main() {

    PieceTable* table = new PieceTable();

    table->insert("Something about text", 0);

    printTable(table);

    table->deleteText(0, 5);

    printTable(table);


    table->deleteText(7, 10);

    printTable(table);

    table->insert("Hello", 5);

    printTable(table);

    table->deleteText(0, table->getSize());

    printTable(table);

    table->insert("Something new", 0);

    printTable(table);

    undoAndPrint(table, 6);
    redoAndPrint(table, 6);

    delete table;

    return 0;
}
