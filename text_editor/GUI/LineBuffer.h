//
// Created by bbard on 7/4/2024.
//

#ifndef TEXT_EDITOR_LINEBUFFER_H
#define TEXT_EDITOR_LINEBUFFER_H

#include "../CodeFolding/CodeBlock.h"
#include "../PieceTable/PieceTableInstance.h"
#include "TextCoordinates.h"
#include "../SyntaxHiglighting/TextHighlighter.h"

#include <numeric>
#include <sstream>

class LineBuffer {
public:
    LineBuffer(PieceTableInstance* pieceTableInstance);
    ~LineBuffer();

    void getLines(int lineIndex = -1);
    void updateHiddenForBlock(CodeBlock* block);
    void clearBlocks();

    size_t textCoordinatesToBufferIndex(const TextCoordinates& coords) const;
    TextCoordinates bufferIndexToTextCoordinates(const size_t& index);

    std::string& lineAt(size_t index) const;
    std::vector<ThemeColor>& getColorMap(size_t index) const;
    const std::vector<CodeBlock*>& getBlocks() const;
    const std::vector<bool>& getHidden() const;
    const size_t getRowsShowing(size_t lineIndex) const;
    bool lineStarsWithTab(const size_t lineIndex) const;
    bool isPlainText() const;
    const size_t getLinesSize() const;
    const size_t getCharSize() const;
    const LanguageMode getLanguageMode() const;
    bool isEmpty() const;


    void setLanguageMode(const LanguageMode mode);
private:
    void updateCharSize();
    void updateColorMap();
    void updateBlocks(int lineSizeDiff, int lineIndex);
    void updateFoldedBlocks(int lineSizeDiff, int lineIndex);

    int findBlock(CodeBlock* block);
    int findGreaterOrEqualBlock(size_t lineIndex);
    void writeInHidden(CodeBlock* block);

    void markMultilineComments();

    static std::string m_emptyLine;
    static std::vector<ThemeColor> m_emptyMap;
    size_t m_charSize;
    std::vector<std::string>* m_lines;
    std::vector<std::vector<ThemeColor>>* m_colorMap;
    std::vector<CodeBlock*>* m_blocks;
    std::vector<bool>* m_hidden;
    PieceTableInstance* m_pieceTableInstance;
    LanguageMode m_mode;
};


#endif //TEXT_EDITOR_LINEBUFFER_H
