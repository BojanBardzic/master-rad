//
// Created by bbard on 6/30/2024.
//

#ifndef TEXT_EDITOR_TEXTEDITOR_H
#define TEXT_EDITOR_TEXTEDITOR_H

#include "TextBox.h"
#include "ThemeManager.h"
#include "../CodeSnippets/SnippetManager.h"

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
    bool saveSnippet(char* name);
    void updateTextBoxMargins();

    void handleKeyboardInput();
    void handleMouseInput();

    void newFile();
    void open();
    void save();
    void saveAs();

    inline bool isKeyPressed(ImGuiKey&& key, bool repeat = true);
    bool isWindowSizeChanged();

    void snippetsDialog();
    void saveSnippetDialog();
    void checkSnippetDeleteDialog(char** namesList, int& itemCurrent);
    static std::string openFileDialog();
    static std::string saveFileDialog();
    int fileNotSavedWarningMessageBox();
    int handleFileNotSaved();

    void pushDialogStyle();
    void popDialogStyle();

    static std::string wStringToString(const std::wstring& wstring);

    TextBox* m_activeTextBox;
    TextBox* m_inactiveTextBox;
    TextBox* m_textBox;
    TextBox* m_secondTextBox;
    Font* m_menuFont;
    Font* m_textFont;
    ImVec2 m_size;
    char* m_saveSnippetBuffer;
    size_t m_saveSnippetBufferSize;
    bool m_splitScreen = false;
    bool m_snippetDialogActive = false;
    bool m_checkDialogActive = false;
    bool m_saveSnippetDialogActive = false;
    bool m_nameIncorrectMessageActive = false;
    bool m_menuActive = false;
    const std::string m_textFontName = "Consolas";
    const std::string m_menuFontName = "Segoe UI";
    const float m_textFontSize = 17.0f;
    const float m_menuFontSize = 19.0f;
    const float m_defaultWidth = 200.0f;
    const float m_defaultHeight = 400.0f;
    const static ImGuiWindowFlags m_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove
                                            | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;

};


#endif //TEXT_EDITOR_TEXTEDITOR_H
