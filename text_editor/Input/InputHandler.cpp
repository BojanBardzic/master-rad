//
// Created by bbard on 6/27/2024.
//

#include "InputHandler.h"

InputHandler::InputHandler(TextBox *textBox) : m_textBox(textBox) {}

void InputHandler::handleKeyboardInput() {
    auto io = ImGui::GetIO();

    if (ImGui::IsWindowFocused()) {

        if (isKeyPressed(ImGuiKey_RightArrow)) {
            m_textBox->moveCursorRight();
        } else if (isKeyPressed(ImGuiKey_LeftArrow)) {
            m_textBox->moveCursorLeft();
        } else if (isKeyPressed(ImGuiKey_UpArrow)) {
            m_textBox->moveCursorUp();
        } else if (isKeyPressed(ImGuiKey_DownArrow)) {
            m_textBox->moveCursorDown();
        } else if (isKeyPressed(ImGuiKey_End)) {
            m_textBox->moveCursorToEnd();
        } else if (isKeyPressed(ImGuiKey_Home)) {
            m_textBox->moveCursorToBeginning();
        } else if (isKeyPressed(ImGuiKey_Enter)) {
            m_textBox->enterChar("\n");
        } else if (isKeyPressed(ImGuiKey_Tab)) {
            m_textBox->enterChar("\t");
        } else if (isKeyPressed(ImGuiKey_Backspace)) {
            m_textBox->backspace();
        } else if (isKeyPressed(ImGuiKey_Delete)) {
            m_textBox->deleteChar();
        }

        if (!io.InputQueueCharacters.empty()) {
            for (ImWchar c : io.InputQueueCharacters) {
                char ch = (char) c;

                if (std::isprint(ch)) {
                    m_textBox->enterChar(std::string(1, ch));
                }
            }
        }
    }

}

bool InputHandler::isKeyPressed(ImGuiKey&& key) {
    return ImGui::IsKeyPressed(ImGui::GetKeyIndex(key));
}
