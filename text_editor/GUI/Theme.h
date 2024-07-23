//
// Created by bbard on 7/10/2024.
//

#ifndef TEXT_EDITOR_THEME_H
#define TEXT_EDITOR_THEME_H

#include "imgui.h"
#include <string>

class Theme {
public:
    Theme(std::string name, ImColor backgroundColor, ImColor textColor, ImColor cursorColor, ImColor selectColor,
          ImColor selectTextColor, ImColor scrollbarPrimaryColor, ImColor scrollbarSecondaryColor);
    ~Theme();

    const std::string& getName() const;
    const ImColor& getBackGroundColor() const;
    const ImColor& getTextColor() const;
    const ImColor& getCursorColor() const;
    const ImColor& getSelectColor() const;
    const ImColor& getSelectTextColor() const;
    const ImColor& getScrollbarPrimaryColor() const;
    const ImColor& getScrollbarSecondaryColor() const;
private:
    std::string m_name;
    ImColor m_backgroundColor;
    ImColor m_textColor;
    ImColor m_cursorColor;
    ImColor m_selectColor;
    ImColor m_selectTextColor;
    ImColor m_scrollbarPrimaryColor;
    ImColor m_scrollbarSecondaryColor;
};


#endif //TEXT_EDITOR_THEME_H
