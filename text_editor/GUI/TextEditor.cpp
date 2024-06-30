//
// Created by bbard on 6/30/2024.
//

#include "TextEditor.h"

TextEditor::TextEditor() {
    m_textBox = new TextBox();
    m_textBox->setWidth(500.0f);
    m_inputHandler = new InputHandler(m_textBox);
}

TextEditor::~TextEditor() {
    delete m_inputHandler;
    delete m_textBox;
}

void TextEditor::draw() {
    bool use_work_area = true;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(use_work_area ? viewport->WorkPos : viewport->Pos);
    ImGui::SetNextWindowSize(use_work_area ? viewport->WorkSize : viewport->Size);

    ImGui::Begin("TextEditor", 0, m_flags);
    m_inputHandler->handleKeyboardInput();
    m_textBox->draw();
    ImGui::End();
}




