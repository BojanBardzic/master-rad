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
        } else if (ctrl && isKeyPressed(ImGuiKey_X)) {
            m_textBox->cut();
        } else if (ctrl && isKeyPressed(ImGuiKey_C)) {
            m_textBox->copy();
        } else if (ctrl && isKeyPressed(ImGuiKey_V)) {
            m_textBox->paste();
        }

        if (!io.InputQueueCharacters.empty()) {
            for (ImWchar c : io.InputQueueCharacters) {
                char ch = (char) c;

                if (std::isprint(ch)) {
                    std::cerr << "Entering char: " << ch << std::endl;
                    m_textBox->enterChar(std::string(1, ch));
                }
            }
            io.InputQueueCharacters.resize(0);
        }
    }

}

void InputHandler::handleMouseInput() {
    auto position = ImGui::GetMousePos();
    auto mouseWheel = ImGui::GetIO().MouseWheel;
    auto shift = ImGui::GetIO().KeyShift;

    if (ImGui::GetMouseClickedCount(ImGuiMouseButton_Left) > 0) {
        if (isInsideHorizontalScrollbar(position)) {
            m_textBox->horizontalBarClick(position);
        } else if (isInsideVerticalScrollbar(position)) {
            m_textBox->verticalBarClick(position);
        } if (isInsideTextBox(position)) {
            m_textBox->moveCursorToMousePosition(position);
        }
    } else if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        auto delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);

        if (isInsideHorizontalScrollbar(position)) {
            m_textBox->horizontalScroll(position, delta);
        } else if (isInsideVerticalScrollbar(position)) {
            m_textBox->verticalScroll(position, delta);
        } else  if (isInsideTextBox(position)) {
            m_textBox->setMouseSelection(position, delta);
        }
    } else if (mouseWheel != 0.0f) {
        m_textBox->mouseWheelScroll(shift, mouseWheel);
    }
}

bool InputHandler::isKeyPressed(ImGuiKey&& key) {
    return ImGui::IsKeyPressed(ImGui::GetKeyIndex(key));
}

bool InputHandler::isInsideTextBox(ImVec2& mousePosition) {
    auto topLeft = ImGui::GetCursorScreenPos();
    auto bottomRight = ImVec2(topLeft.x + m_textBox->getWidth(), topLeft.y + m_textBox->getHeight());
    auto rect = MyRectangle(topLeft, bottomRight);

    return MyRectangle::isInsideRectangle(rect, mousePosition);
}

bool InputHandler::isInsideHorizontalScrollbar(ImVec2 &mousePosition) {
    return MyRectangle::isInsideRectangle(m_textBox->getHScrollbarRect(), mousePosition);
}

bool InputHandler::isInsideVerticalScrollbar(ImVec2 &mousePosition) {
    return MyRectangle::isInsideRectangle(m_textBox->getVScrollbarRect(), mousePosition);
}


