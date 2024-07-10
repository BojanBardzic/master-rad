//
// Created by bbard on 7/10/2024.
//

#include "Theme.h"

Theme::Theme(ImColor backgroundColor, ImColor textColor, ImColor cursorColor, ImColor selectColor,
             ImColor selectTextColor, ImColor scrollbarPrimaryColor, ImColor scrollbarSecondaryColor)
             : m_backgroundColor(backgroundColor), m_textColor(textColor), m_cursorColor(cursorColor),
               m_selectColor(selectColor), m_selectTextColor(selectTextColor),
               m_scrollbarPrimaryColor(scrollbarPrimaryColor), m_scrollbarSecondaryColor(scrollbarSecondaryColor) {}

Theme::~Theme() {}

const ImColor &Theme::getBackGroundColor() const { return m_backgroundColor; }

const ImColor &Theme::getTextColor() const { return m_textColor; }

const ImColor &Theme::getCursorColor() const { return m_cursorColor; }

const ImColor &Theme::getSelectColor() const { return m_selectColor; }

const ImColor &Theme::getSelectTextColor() const { return m_selectTextColor; }

const ImColor &Theme::getScrollbarPrimaryColor() const { return m_scrollbarPrimaryColor; }

const ImColor &Theme::getScrollbarSecondaryColor() const { return m_scrollbarSecondaryColor; }
