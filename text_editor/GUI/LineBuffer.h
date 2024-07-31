//
// Created by bbard on 7/4/2024.
//

#ifndef TEXT_EDITOR_LINEBUFFER_H
#define TEXT_EDITOR_LINEBUFFER_H

#include <numeric>
#include <sstream>
#include "../PieceTable/PieceTableInstance.h"
#include "TextCoordinates.h"
#include "TextHighlighter.h"

class LineBuffer {
public:
    LineBuffer(PieceTableInstance* pieceTableInstance);
    ~LineBuffer();

    void getLines();
    size_t textCoordinatesToBufferIndex(const TextCoordinates& coords) const;
    TextCoordinates bufferIndexToTextCoordinates(const size_t& index);

    std::string& lineAt(size_t index) const;
    std::vector<ThemeColor>& getColorMap(size_t index) const;
    bool lineStarsWithTab(const size_t lineIndex) const;
    const size_t getLinesSize() const;
    const size_t getCharSize() const;
    const LanguageMode getLanguageMode() const;
    bool isEmpty() const;


    void setLanguageMode(const LanguageMode mode);
private:
    void updateCharSize();
    void updateColorMap();

    static std::string m_emptyLine;
    static std::vector<ThemeColor> m_emptyMap;
    size_t m_charSize;
    std::vector<std::string>* m_lines;
    std::vector<std::vector<ThemeColor>>* m_colorMap;
    PieceTableInstance* m_pieceTableInstance;
    LanguageMode m_mode;
};


#endif //TEXT_EDITOR_LINEBUFFER_H
