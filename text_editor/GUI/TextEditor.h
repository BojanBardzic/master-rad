//
// Created by bbard on 6/30/2024.
//

#ifndef TEXT_EDITOR_TEXTEDITOR_H
#define TEXT_EDITOR_TEXTEDITOR_H

#include "TextBox.h"
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

    void handleKeyboardInput();
    void handleMouseInput();
    inline bool isKeyPressed(ImGuiKey&& key, bool repeat = true);
    bool isInsideTextBox(ImVec2& mousePosition);
    bool isInsideHorizontalScrollbar(ImVec2& mousePosition);
    bool isInsideVerticalScrollbar(ImVec2& mousePosition);

    void newFile();
    void open();
    void save();
    void saveAs();

    std::string openFileDialog();
    std::string saveFileDialog();
    int fileNotSavedWarningMessageBox();
    int handleFileNotSaved();
    std::string wStringToString(const std::wstring& wstring);

    TextBox* m_textBox;
    Font* m_menuFont;
    bool m_clickedOnMenu;
    const std::string m_menuFontName = "Segoe UI";
    const float m_menuFontSize = 19.0f;
    const static ImGuiWindowFlags m_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove
                                            | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;

};


#endif //TEXT_EDITOR_TEXTEDITOR_H
