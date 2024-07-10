//
// Created by bbard on 7/8/2024.
//

#ifndef TEXT_EDITOR_SCROLL_H
#define TEXT_EDITOR_SCROLL_H

#include "LineBuffer.h"
#include "Font.h"
#include "Cursor.h"
#include "MyRectangle.h"

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
    const MyRectangle& getHScrollbarRect() const;
    const MyRectangle& getVScrollbarRect() const;
    const MyRectangle& getHScrollSelectRect() const;
    const MyRectangle& getVScrollSelectRect() const;

    void setXScroll(const float& xScroll);
    void setYScroll(const float& yScroll);
    void setHScrollbarRect(const MyRectangle &hScrollbarRect);
    void setVScrollbarRect(const MyRectangle &vScrollbarRect);
    void setHScrollSelectRect(const MyRectangle &hScrollSelectRect);
    void setVScrollSelectRect(const MyRectangle &vScrollSelectRect);
private:
    LineBuffer* m_lineBuffer;
    Cursor* m_cursor;
    Font* m_font;
    MyRectangle m_hScrollbarRect;
    MyRectangle m_vScrollbarRect;
    MyRectangle m_hScrollSelectRect;
    MyRectangle m_vScrollSelectRect;
    float m_maxXScroll;
    float m_maxYScroll;
    float m_xScroll;
    float m_yScroll;
    bool m_init;
};


#endif //TEXT_EDITOR_SCROLL_H
