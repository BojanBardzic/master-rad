//
// Created by bbard on 6/30/2024.
//

#ifndef TEXT_EDITOR_TEXTEDITOR_H
#define TEXT_EDITOR_TEXTEDITOR_H

#include "TextBox.h"
#include "ThemeManager.h"
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
    void drawStatusBar();

    void toggleSplitScreen();
    void updateTextBoxMargins();

    void handleKeyboardInput();
    void handleMouseInput();

    inline bool isKeyPressed(ImGuiKey&& key, bool repeat = true);
    bool isWindowSizeChanged();

    void newFile();
    void open();
    void save();
    void saveAs();

    static std::string openFileDialog();
    static std::string saveFileDialog();
    int fileNotSavedWarningMessageBox();
    int handleFileNotSaved();

    static std::string wStringToString(const std::wstring& wstring);

    TextBox* m_activeTextBox;
    TextBox* m_inactiveTextBox;
    TextBox* m_textBox;
    TextBox* m_secondTextBox;
    Font* m_menuFont;
    ImVec2 m_size;
    bool m_splitScreen;
    bool m_menuActive;
    const std::string m_textFontName = "Consolas";
    const std::string m_menuFontName = "Segoe UI";
    const float m_defaultWidth = 200.0f;
    const float m_defaultHeight = 400.0f;
    const float m_menuFontSize = 19.0f;
    const static ImGuiWindowFlags m_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove
                                            | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;

};


#endif //TEXT_EDITOR_TEXTEDITOR_H
