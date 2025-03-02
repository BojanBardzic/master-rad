//
// Created by bbard on 7/10/2024.
//

#include "MyRectangle.h"

MyRectangle::MyRectangle() : m_topLeft(ImVec2(0.0f, 0.0f)), m_bottomRight(ImVec2(0.0f, 0.0f)) {}

MyRectangle::MyRectangle(ImVec2 topLeft, ImVec2 bottomRight) : m_topLeft(topLeft), m_bottomRight(bottomRight) {}

MyRectangle::~MyRectangle() {}

const ImVec2& MyRectangle::getTopLeft() const { return m_topLeft; }

const ImVec2& MyRectangle::getBottomRight() const { return m_bottomRight; }

void MyRectangle::setTopLeft(const ImVec2& topLeft) { m_topLeft = topLeft; }

void MyRectangle::setBottomRight(const ImVec2& bottomRight) { m_bottomRight = bottomRight; }

bool MyRectangle::isInsideRectangle(const MyRectangle& rect, const ImVec2 &point) {
    return point.x >= rect.m_topLeft.x && point.x <= rect.m_bottomRight.x && point.y >= rect.m_topLeft.y && point.y <= rect.m_bottomRight.y;
}

bool MyRectangle::isAboveRectangle(const MyRectangle& rect, const ImVec2& point) {
    return point.y < rect.m_topLeft.y;
}

bool MyRectangle::isBelowRectangle(const MyRectangle& rect, const ImVec2& point) {
    return point.y > rect.m_bottomRight.y;
}

bool MyRectangle::isLeftOfRectangle(const MyRectangle &rect, const ImVec2& point) {
    return point.x < rect.m_topLeft.x;
}

bool MyRectangle::isRightOfRectangle(const MyRectangle &rect, const ImVec2& point) {
    return point.x > rect.m_bottomRight.x;
}

bool MyRectangle::areRectanglesIntersecting(const MyRectangle &rect1, const MyRectangle &rect2) {
    if (rect1.m_topLeft.x > rect2.m_bottomRight.x || rect1.m_bottomRight.x < rect2.m_topLeft.x ||
        rect1.m_topLeft.y > rect2.m_bottomRight.y || rect1.m_bottomRight.y < rect2.m_topLeft.y) {
        return false;
    }

    return true;
}


