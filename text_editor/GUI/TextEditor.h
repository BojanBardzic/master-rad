//
// Created by bbard on 6/30/2024.
//

#ifndef TEXT_EDITOR_TEXTEDITOR_H
#define TEXT_EDITOR_TEXTEDITOR_H

#include "TextBox.h"
#include "../Input/InputHandler.h"

class TextEditor {
public:
    TextEditor();
    ~TextEditor();

    void draw();
private:
    TextBox* m_textBox;
    InputHandler* m_inputHandler;
    const static ImGuiWindowFlags m_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove
                                            | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;
};


#endif //TEXT_EDITOR_TEXTEDITOR_H
