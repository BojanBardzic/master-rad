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

    static std::string openFileDialog();
    static std::string saveFileDialog();
    int fileNotSavedWarningMessageBox();
    int handleFileNotSaved();

    static std::string wStringToString(const std::wstring& wstring);

    TextBox* m_textBox;
    Font* m_menuFont;
    bool m_clickedOnMenu;
    const std::string m_textFontName = "Consolas";
    const std::string m_menuFontName = "Segoe UI";
    const ThemeName m_defaultTheme = ThemeName::Light;
    const float m_defaultWidth = 200.0f;
    const float m_defaultHeight = 400.0f;
    const float m_menuFontSize = 19.0f;
    const static ImGuiWindowFlags m_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove
                                            | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar;

};


#endif //TEXT_EDITOR_TEXTEDITOR_H
