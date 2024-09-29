//
// Created by bbard on 9/9/2024.
//

#ifndef TEXT_EDITOR_CODEBLOCK_H
#define TEXT_EDITOR_CODEBLOCK_H

#include "../GUI/TextCoordinates.h"

class CodeBlock {
public:
    friend std::ostream& operator<<(std::ostream& out, const CodeBlock& block);

    bool operator==(const CodeBlock& other) const;
    bool operator!=(const CodeBlock& other) const;
    bool operator<(const CodeBlock& other) const;
    bool operator>(const CodeBlock& other) const;

    CodeBlock(TextCoordinates& start, TextCoordinates& end);
    CodeBlock(const CodeBlock& block);
    ~CodeBlock();

    const TextCoordinates& getStart() const;
    const TextCoordinates& getEnd() const;
    const bool isFolded() const;

    void setStart(const TextCoordinates& start);
    void setEnd(const TextCoordinates& end);
    void setFolded(const bool folded);
private:
    TextCoordinates m_start;
    TextCoordinates m_end;
    bool m_folded;
};


#endif //TEXT_EDITOR_CODEBLOCK_H
