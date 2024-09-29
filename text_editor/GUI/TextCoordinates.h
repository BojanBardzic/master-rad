//
// Created by bbard on 7/4/2024.
//

#ifndef TEXT_EDITOR_TEXTCOORDINATES_H
#define TEXT_EDITOR_TEXTCOORDINATES_H

#include <iostream>

class TextCoordinates {
public:
    friend std::ostream& operator<<(std::ostream& out, const TextCoordinates& coords);
    
    TextCoordinates& operator=(const TextCoordinates& other);
    bool operator==(const TextCoordinates& other) const;
    bool operator!=(const TextCoordinates& other) const;
    bool operator<(const TextCoordinates& other) const;
    bool operator>(const TextCoordinates& other) const;
    bool operator<=(const TextCoordinates& other) const;
    bool operator>=(const TextCoordinates& other) const;

    TextCoordinates(size_t row = 1, size_t col = 1);
    ~TextCoordinates();
public:
    size_t m_row;
    size_t m_col;
};


#endif //TEXT_EDITOR_TEXTCOORDINATES_H
