//
// Created by bbard on 7/10/2024.
//

#ifndef TEXT_EDITOR_THEME_H
#define TEXT_EDITOR_THEME_H

#include "imgui.h"

class Theme {
public:
    Theme(ImColor backgroundColor, ImColor textColor, ImColor cursorColor, ImColor selectColor,
          ImColor selectTextColor, ImColor scrollbarPrimaryColor, ImColor scrollbarSecondaryColor);
    ~Theme();

    const ImColor& getBackGroundColor() const;
    const ImColor& getTextColor() const;
    const ImColor& getCursorColor() const;
    const ImColor& getSelectColor() const;
    const ImColor& getSelectTextColor() const;
    const ImColor& getScrollbarPrimaryColor() const;
    const ImColor& getScrollbarSecondaryColor() const;
private:
    ImColor m_backgroundColor;
    ImColor m_textColor;
    ImColor m_cursorColor;
    ImColor m_selectColor;
    ImColor m_selectTextColor;
    ImColor m_scrollbarPrimaryColor;
    ImColor m_scrollbarSecondaryColor;
};


#endif //TEXT_EDITOR_THEME_H
