//
// Created by bbard on 6/30/2024.
//

#ifndef TEXT_EDITOR_TEXTEDITOR_H
#define TEXT_EDITOR_TEXTEDITOR_H

#include "TextBox.h"
#include "../Input/InputHandler.h"
#include <cstdlib>
#include <sstream>
#include <windows.h>
#include <shobjidl.h>

class TextEditor {
public:
    TextEditor();
    ~TextEditor();

    void draw();
private:
    void drawMenu();
    std::string openFileDialog();

    TextBox* m_textBox;
    InputHandler* m_inputHandler;
    Font* m_menuFont;
    bool m_clickedOnMenu;
    const std::string m_menuFontName = "Segoe UI";
    const float m_menuFontSize = 19.0f;
    const static ImGuiWindowFlags m_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove
                                            | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;

};


#endif //TEXT_EDITOR_TEXTEDITOR_H
