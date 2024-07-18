//
// Created by bbard on 7/11/2024.
//

#ifndef TEXT_EDITOR_INSERTBUFFER_H
#define TEXT_EDITOR_INSERTBUFFER_H

#include <string>

class InsertBuffer {
public:
    InsertBuffer();
    ~InsertBuffer();

    const size_t& getStartIndex() const;
    const size_t& getEndIndex() const;
    bool isFlushed() const;
    const std::string& getContent() const;

    void setStartIndex(const size_t& startIndex);
    void setEndIndex(const size_t& endIndex);
    void setFlushed(bool flushed);
    void appendToContent(char c);
    void clearContent();
private:
    bool m_flushed;
    size_t m_startIndex;
    size_t m_endIndex;
    std::string m_content;
};


#endif //TEXT_EDITOR_INSERTBUFFER_H
