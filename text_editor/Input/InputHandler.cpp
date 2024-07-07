//
// Created by bbard on 6/27/2024.
//

#include "InputHandler.h"

InputHandler::InputHandler(TextBox *textBox) : m_textBox(textBox) {}

void InputHandler::handleKeyboardInput() {
    auto io = ImGui::GetIO();

    auto ctrl = io.KeyCtrl;
    auto shift = io.KeyShift;

    if (ImGui::IsWindowFocused()) {

        if (isKeyPressed(ImGuiKey_RightArrow)) {
            m_textBox->moveCursorRight(shift);
        } else if (isKeyPressed(ImGuiKey_LeftArrow)) {
            m_textBox->moveCursorLeft(shift);
        } else if (isKeyPressed(ImGuiKey_UpArrow)) {
            m_textBox->moveCursorUp(shift);
        } else if (isKeyPressed(ImGuiKey_DownArrow)) {
            m_textBox->moveCursorDown(shift);
        } else if (isKeyPressed(ImGuiKey_End)) {
            m_textBox->moveCursorToEnd(shift);
        } else if (isKeyPressed(ImGuiKey_Home)) {
            m_textBox->moveCursorToBeginning(shift);
        } else if (isKeyPressed(ImGuiKey_Enter)) {
            m_textBox->enterChar("\n");
        } else if (isKeyPressed(ImGuiKey_Tab)) {
            m_textBox->enterChar("\t");
        } else if (isKeyPressed(ImGuiKey_Backspace)) {
            m_textBox->backspace();
        } else if (isKeyPressed(ImGuiKey_Delete)) {
            m_textBox->deleteChar();
        } else if (ctrl && isKeyPressed(ImGuiKey_KeypadAdd)) {
            m_textBox->increaseFontSize();
        } else if (ctrl && isKeyPressed(ImGuiKey_KeypadSubtract)) {
            m_textBox->decreaseFontSize();
        } else if (ctrl && isKeyPressed(ImGuiKey_A)) {
            m_textBox->selectAll();
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
