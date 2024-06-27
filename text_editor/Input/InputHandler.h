//
// Created by bbard on 6/27/2024.
//

#ifndef TEXT_EDITOR_INPUTHANDLER_H
#define TEXT_EDITOR_INPUTHANDLER_H

#include "../GUI/TextBox.h"

class InputHandler {
public:
    InputHandler(TextBox* textBox);

    void handleInput();

private:
    inline bool isKeyPressed(ImGuiKey&& key);

    TextBox* m_textBox;
};


#endif //TEXT_EDITOR_INPUTHANDLER_H
