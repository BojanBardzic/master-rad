//
// Created by bbard on 7/12/2024.
//

#ifndef TEXT_EDITOR_DELETEBUFFER_H
#define TEXT_EDITOR_DELETEBUFFER_H


class DeleteBuffer {
public:
    DeleteBuffer();
    ~DeleteBuffer();

    const size_t& getStartIndex() const;
    const size_t& getEndIndex() const;
    const size_t& getDeleteIndex() const;
    const size_t getDeleteSize() const;
    bool isFlushed() const;

    void setStartIndex(const size_t& startIndex);
    void setEndIndex(const size_t& endIndex);
    void setDeleteIndex(const size_t& deleteIndex);
    void setFlushed(bool flushed);
private:
    size_t m_startIndex;
    size_t m_endIndex;
    size_t m_deleteIndex;
    bool m_flushed;
};


#endif //TEXT_EDITOR_DELETEBUFFER_H
