//
// Created by bbard on 6/30/2024.
//

#include "TextEditor.h"

TextEditor::TextEditor() : m_menuActive(false), m_splitScreen(false), m_size({0.0f, 0.0f}) {
    // Setup font and theme
    FontManager::init();
    m_menuFont = new Font(m_menuFontName, m_menuFontSize);

    ThemeManager::init();

    m_textBox = new TextBox(m_defaultWidth, m_defaultHeight, m_textFontName);
    m_textBox->setWidth(500.0f);

    m_secondTextBox = new TextBox(m_defaultWidth, m_defaultHeight, m_textFontName, m_textBox->getPieceTableInstance());

    m_activeTextBox = m_textBox;
    m_inactiveTextBox = m_secondTextBox;

    std::cerr << "Exiting constructor" << std::endl;
}

TextEditor::~TextEditor() {
    delete m_textBox;
    delete m_secondTextBox;
}

void TextEditor::draw() {
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    ImGui::Begin("TextEditor", 0, m_flags);
    // Draw the top menu
    drawMenu();

    // Handle inputs;
    handleKeyboardInput();
    if (!m_menuActive)
        handleMouseInput();

   if (m_splitScreen)
       m_inactiveTextBox->getLines();

    // Draw the text box
    m_textBox->draw();
    if (m_splitScreen)
        m_secondTextBox->draw();

    if (isWindowSizeChanged())
        updateTextBoxMargins();


    // Draw the status bar
    drawStatusBar();

    ImGui::End();
}

void TextEditor::drawMenu() {
    ImGui::PushFont(m_menuFont->getFont());
    bool clickedOnMenu = false;

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            clickedOnMenu = true;
            m_menuActive = true;

            if (ImGui::MenuItem("New", "Ctrl+N")) {
                newFile();
            }
            if (ImGui::MenuItem("Open...", "Ctrl+O")) {
                open();
            }
            if (ImGui::MenuItem("Save", "Ctrl+S")) {
                save();
            }
            if (ImGui::MenuItem("Save as...", "Ctrl+Shift+S")) {
                saveAs();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit")) {
            clickedOnMenu = true;
            m_menuActive = true;

            if (ImGui::MenuItem("Undo", "Ctrl+Z", false, !m_activeTextBox->isUndoEmpty())) {
                m_activeTextBox->undo();
            }
            if (ImGui::MenuItem("Redo", "Ctrl+Y", false, !m_activeTextBox->isRedoEmpty())) {
                m_activeTextBox->redo();
            }
            if (ImGui::MenuItem("Cut", "Ctrl+X", false, m_activeTextBox->isSelectionActive())) {
                m_activeTextBox->cut();
            }
            if (ImGui::MenuItem("Copy", "Ctrl+C", false, m_activeTextBox->isSelectionActive())) {
                m_activeTextBox->copy();
            }
            if (ImGui::MenuItem("Paste", "Ctrl+V")) {
                m_activeTextBox->paste();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Theme")) {
            clickedOnMenu = true;
            m_menuActive = true;

            if (ImGui::MenuItem("Light", "", false, ThemeManager::getTheme()->getName() != ThemeName::Light)) {
                ThemeManager::setTheme(ThemeName::Light);
            }
            if (ImGui::MenuItem("Dark", "", false, ThemeManager::getTheme()->getName() != ThemeName::Dark)) {
                ThemeManager::setTheme(ThemeName::Dark);
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Tools")) {
            clickedOnMenu = true;
            m_menuActive = true;

            if (ImGui::MenuItem("Edit narrowing", "Ctrl+W", m_activeTextBox->isWriteSelectionActive())) {
                if (m_activeTextBox->isWriteSelectionActive())
                    m_activeTextBox->deactivateWriteSelection();
                else
                    m_activeTextBox->activateWriteSelection();
            }
            if (ImGui::MenuItem("Rectangular selection", "Ctrl+R", m_textBox->isRectangularSelectionActive())) {
                m_textBox->toggleRectangularSelection();
                m_secondTextBox->toggleRectangularSelection();
            }

            if (ImGui::MenuItem("Split screen", "", m_splitScreen)) {
                toggleSplitScreen();
            }

            ImGui::EndMenu();
        }

        if (!clickedOnMenu)
            m_menuActive = false;

        ImGui::EndMainMenuBar();
    }

    ImGui::PopFont();
}

void TextEditor::drawStatusBar() {
    float offset = 15.0f;

    ImGui::PushFont(m_menuFont->getFont());

    auto topLeft = m_textBox->getTopLeft();
    auto textPosition = ImVec2(topLeft.x, topLeft.y + m_activeTextBox->getHeight() + offset + 5.0f);
    auto text = m_activeTextBox->getStatusBarText();
    ImGui::GetWindowDrawList()->AddText(textPosition, ImColor(255, 255, 255), text.c_str());

    ImGui::PopFont();
}

void TextEditor::toggleSplitScreen() {
    m_splitScreen = !m_splitScreen;

    if (!m_splitScreen)
        m_activeTextBox = m_textBox;

    updateTextBoxMargins();
}

void TextEditor::updateTextBoxMargins() {

    if (m_splitScreen) {
        auto halfWidth = m_size.x / 2.0f;

        m_textBox->setBottomRightMargin({5.0f + halfWidth, 5.0f});
        m_secondTextBox->setTopLeftMargin({halfWidth, 0.0f});
    } else {
        m_textBox->setBottomRightMargin({5.0f, 5.0f});
    }
}

void TextEditor::handleKeyboardInput() {
    auto io = ImGui::GetIO();

    auto ctrl = io.KeyCtrl;
    auto shift = io.KeyShift;

    if (ImGui::IsWindowFocused()) {

        if (isKeyPressed(ImGuiKey_RightArrow)) {
            m_activeTextBox->moveCursorRight(shift);
        } else if (isKeyPressed(ImGuiKey_LeftArrow)) {
            m_activeTextBox->moveCursorLeft(shift);
        } else if (isKeyPressed(ImGuiKey_UpArrow)) {
            m_activeTextBox->moveCursorUp(shift);
        } else if (isKeyPressed(ImGuiKey_DownArrow)) {
            m_activeTextBox->moveCursorDown(shift);
        } else if (isKeyPressed(ImGuiKey_End)) {
            m_activeTextBox->moveCursorToEnd(shift);
        } else if (isKeyPressed(ImGuiKey_Home)) {
            m_activeTextBox->moveCursorToBeginning(shift);
        } else if (isKeyPressed(ImGuiKey_Enter)) {
            m_activeTextBox->enterChar('\n');
        } else if (isKeyPressed(ImGuiKey_Tab)) {
            m_activeTextBox->tab(shift);
        } else if (isKeyPressed(ImGuiKey_Backspace)) {
            m_activeTextBox->backspace();
        } else if (shift && isKeyPressed(ImGuiKey_Delete)) {
            m_activeTextBox->deleteLine();
        } else if (isKeyPressed(ImGuiKey_Delete)) {
            m_activeTextBox->deleteChar();
        } else if (ctrl && isKeyPressed(ImGuiKey_KeypadAdd)) {
            m_activeTextBox->increaseFontSize();
        } else if (ctrl && isKeyPressed(ImGuiKey_KeypadSubtract)) {
            m_activeTextBox->decreaseFontSize();
        } else if (isKeyPressed(ImGuiKey_Escape)) {
            m_activeTextBox->deactivateWriteSelection();
        } else if (ctrl && isKeyPressed(ImGuiKey_A)) {
            m_activeTextBox->selectAll();
        } else if (ctrl && isKeyPressed(ImGuiKey_X)) {
            m_activeTextBox->cut();
        } else if (ctrl && isKeyPressed(ImGuiKey_C)) {
            m_activeTextBox->copy();
        } else if (ctrl && isKeyPressed(ImGuiKey_V)) {
            m_activeTextBox->paste();
        } else if (ctrl && isKeyPressed(ImGuiKey_Z)) {
            m_activeTextBox->undo();
        } else if (ctrl && isKeyPressed(ImGuiKey_Y)) {
            m_activeTextBox->redo();
        } else if (ctrl && isKeyPressed(ImGuiKey_N, false)) {
            newFile();
        } else if (ctrl && isKeyPressed(ImGuiKey_O, false)) {
            open();
        } else if (ctrl && shift && isKeyPressed(ImGuiKey_S, false)) {
            saveAs();
        } else if (ctrl && isKeyPressed(ImGuiKey_S, false)) {
            save();
        } else if (ctrl && isKeyPressed(ImGuiKey_W, false)) {
            m_activeTextBox->activateWriteSelection();
        } else if (ctrl && isKeyPressed(ImGuiKey_R, false)) {
            m_activeTextBox->toggleRectangularSelection();
            m_inactiveTextBox->toggleRectangularSelection();
        }

        if (!io.InputQueueCharacters.empty()) {
            for (ImWchar c : io.InputQueueCharacters) {
                char ch = (char) c;

                if (std::isprint(ch)) {
                    std::cerr << "Entering char: " << ch << std::endl;
                    m_activeTextBox->enterChar(ch);
                }
            }
            io.InputQueueCharacters.resize(0);
        }
    }

}

void TextEditor::handleMouseInput() {

    auto position = ImGui::GetMousePos();
    auto mouseWheel = ImGui::GetIO().MouseWheel;
    auto shift = ImGui::GetIO().KeyShift;

    if (ImGui::GetMouseClickedCount(ImGuiMouseButton_Left) > 0) {
        if (m_splitScreen && m_inactiveTextBox->isInsideTextBox(position)) {
            std::swap(m_activeTextBox, m_inactiveTextBox);
        } else if (m_activeTextBox->isInsideHorizontalScrollbar(position)) {
            m_activeTextBox->horizontalBarClick(position);
        } else if (m_activeTextBox->isInsideVerticalScrollbar(position)) {
            m_activeTextBox->verticalBarClick(position);
        } if (m_activeTextBox->isInsideTextBox(position)) {
            m_activeTextBox->moveCursorToMousePosition(position);
        }
    } else if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        auto delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
        auto previousPosition = ImVec2(position.x - delta.x, position.y - delta.y);

        if (m_activeTextBox->isInsideHorizontalScrollbar(position) && m_activeTextBox->isInsideHorizontalScrollbar(previousPosition)) {
            m_activeTextBox->horizontalScroll(position, delta);
        } else if (m_activeTextBox->isInsideVerticalScrollbar(position) && m_activeTextBox->isInsideVerticalScrollbar(previousPosition)) {
            m_activeTextBox->verticalScroll(position, delta);
        } else if (m_activeTextBox->isInsideTextBox(position) && m_activeTextBox->isInsideTextBox(previousPosition)) {
            m_activeTextBox->setMouseSelection(position, delta);
        }
    } else if (mouseWheel != 0.0f) {
        m_activeTextBox->mouseWheelScroll(shift, mouseWheel);
    }
}

bool TextEditor::isKeyPressed(ImGuiKey&& key, bool repeat) {
    return ImGui::IsKeyPressed(ImGui::GetKeyIndex(key), repeat);
}

bool TextEditor::isWindowSizeChanged() {

    if (m_size.x != ImGui::GetWindowWidth() || m_size.y != ImGui::GetWindowHeight()) {
        m_size = {ImGui::GetWindowWidth(), ImGui::GetWindowHeight()};
        return true;
    }

    return false;
}

void TextEditor::newFile() {
    auto id = handleFileNotSaved();
    if (id == IDCANCEL)
        return;

    m_activeTextBox->newFile();
    if (m_splitScreen)
        m_inactiveTextBox->setFile(m_activeTextBox->getFile());
}

void TextEditor::open() {
    auto id = handleFileNotSaved();
    if (id == IDCANCEL)
        return;

    auto path = openFileDialog();

    if (!path.empty()) {
        m_activeTextBox->open(path);
        m_inactiveTextBox->open(path);
    }
}

void TextEditor::save() {
    if (m_activeTextBox->getFile() == nullptr)
        saveAs();
    else {
        m_activeTextBox->save();
        m_inactiveTextBox->save();
    }
}

void TextEditor::saveAs() {
    auto path = saveFileDialog();

    if (!path.empty()) {
        if (path.find_last_of('.') == std::string::npos)
            path += ".txt";
        m_activeTextBox->saveAs(path);
        m_inactiveTextBox->saveAs(path);
    }
}

std::string TextEditor::openFileDialog() {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED |
                                      COINIT_DISABLE_OLE1DDE);

    PWSTR pszFilePath;

    if (SUCCEEDED(hr))
    {
        IFileOpenDialog *pFileOpen;

        // Create the FileOpenDialog object.
        hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_ALL,
                              IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr))
        {
            // Show the Open dialog box.
            hr = pFileOpen->Show(nullptr);

            // Get the file name from the dialog box.
            if (SUCCEEDED(hr))
            {
                IShellItem *pItem;
                hr = pFileOpen->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                    // Display the file name to the user.
                    if (SUCCEEDED(hr))
                    {
                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            } else {
                return "";
            }
            pFileOpen->Release();
        }
        CoUninitialize();
    }

    std::wstringstream stream;
    stream << pszFilePath;

    return wStringToString(stream.str());
}

std::string TextEditor::saveFileDialog() {
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED |
                                      COINIT_DISABLE_OLE1DDE);

    PWSTR pszFilePath;

    if (SUCCEEDED(hr))
    {
        IFileSaveDialog *pFileSave;

        // Create the FileOpenDialog object.
        hr = CoCreateInstance(CLSID_FileSaveDialog, nullptr, CLSCTX_ALL,
                              IID_IFileSaveDialog, reinterpret_cast<void**>(&pFileSave));

        if (SUCCEEDED(hr))
        {
            // Show the Open dialog box.
            hr = pFileSave->Show(nullptr);

            // Get the file name from the dialog box.
            if (SUCCEEDED(hr))
            {
                IShellItem *pItem;
                hr = pFileSave->GetResult(&pItem);
                if (SUCCEEDED(hr))
                {
                    hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                    // Display the file name to the user.
                    if (SUCCEEDED(hr))
                    {
                        CoTaskMemFree(pszFilePath);
                    }
                    pItem->Release();
                }
            } else {
                return "";
            }
            pFileSave->Release();
        }
        CoUninitialize();
    }

    std::wstringstream stream;
    stream << pszFilePath;

    return wStringToString(stream.str());
}

int TextEditor::fileNotSavedWarningMessageBox() {
    std::stringstream stream;
    stream << "Do you want to save changes to "
           << (m_activeTextBox->getFile() == nullptr ? "Untitled" : m_activeTextBox->getFile()->getName())
           << "?";


    auto msgboxID = MessageBox(
            nullptr,
            stream.str().c_str(),
            reinterpret_cast<LPCSTR>(L"TextColor editor"),
            MB_YESNOCANCEL
    );

    return msgboxID;
}

int TextEditor::handleFileNotSaved() {
    if (m_activeTextBox->isDirty()) {
        auto id = fileNotSavedWarningMessageBox();

        if (id == IDYES)
            save();

        return id;
    } else {
        return IDNO;
    }
}

std::string TextEditor::wStringToString(const std::wstring& wstring) {
    size_t len = wcstombs(nullptr, wstring.c_str(), 0) + 1;

    char* result = new char[len];
    wcstombs(result, wstring.c_str(), len);

    return {result};
}









