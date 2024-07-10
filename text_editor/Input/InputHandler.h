//
// Created by bbard on 6/27/2024.
//

#ifndef TEXT_EDITOR_INPUTHANDLER_H
#define TEXT_EDITOR_INPUTHANDLER_H

#include "../GUI/TextBox.h"
#include "../GUI/MyRectangle.h"

class InputHandler {
public:
    InputHandler(TextBox* textBox);

    void handleKeyboardInput();
    void handleMouseInput();

private:
    inline bool isKeyPressed(ImGuiKey&& key);
    bool isInsideTextBox(ImVec2& mousePosition);
    bool isInsideHorizontalScrollbar(ImVec2& mousePosition);
    bool isInsideVerticalScrollbar(ImVec2& mousePosition);

    TextBox* m_textBox;
};


#endif //TEXT_EDITOR_INPUTHANDLER_H
