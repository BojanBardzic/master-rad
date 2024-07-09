//
// Created by bbard on 7/8/2024.
//

#ifndef TEXT_EDITOR_SCROLL_H
#define TEXT_EDITOR_SCROLL_H

#include "LineBuffer.h"
#include "Font.h"
#include "Cursor.h"

class Scroll {
public:
    Scroll(LineBuffer* lineBuffer, Cursor* cursor, Font* font);
    ~Scroll();

    void updateScroll(float& width, float& height);
    void updateXScroll(float& width);
    void updateYScroll(float& height);
    void updateMaxScroll(float& width, float& height);
    void updateMaxXScroll(float& width);
    void updateMaxYScroll(float& height);

    void mouseWheelXScroll(float& mouseWheel);
    void mouseWheelYScroll(float& mouseWheel);

    void init(float& width, float& height);

    bool isInit() const;
    float getXScroll() const;
    float getYScroll() const;
    float getMaxXScroll() const;
    float getMaxYScroll() const;
private:
    LineBuffer* m_lineBuffer;
    Cursor* m_cursor;
    Font* m_font;
    float m_maxXScroll;
    float m_maxYScroll;
    float m_xScroll;
    float m_yScroll;
    bool m_init;
};


#endif //TEXT_EDITOR_SCROLL_H
