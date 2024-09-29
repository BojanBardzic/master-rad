//
// Created by bbard on 7/10/2024.
//

#ifndef TEXT_EDITOR_MYRECTANGLE_H
#define TEXT_EDITOR_MYRECTANGLE_H

#include "imgui.h"

#include <iostream>

class MyRectangle {
public:
    MyRectangle();
    MyRectangle(ImVec2 topLeft, ImVec2 bottomRight);
    ~MyRectangle();

    const ImVec2& getTopLeft() const;
    const ImVec2& getBottomRight() const;

    void setTopLeft(const ImVec2& topLeft);
    void setBottomRight(const ImVec2& bottomRight);

    static bool isInsideRectangle(const MyRectangle& rect,const ImVec2& point);
    static bool isAboveRectangle(const MyRectangle& rect, const ImVec2& point);
    static bool isBelowRectangle(const MyRectangle& rect, const ImVec2& point);
    static bool isLeftOfRectangle(const MyRectangle& rect, const ImVec2& point);
    static bool isRightOfRectangle(const MyRectangle& rect, const ImVec2& point);
private:
    ImVec2 m_topLeft;
    ImVec2 m_bottomRight;
};


#endif //TEXT_EDITOR_MYRECTANGLE_H
