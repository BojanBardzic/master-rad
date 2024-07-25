//
// Created by bbard on 6/30/2024.
//

#include "TextEditor.h"

TextEditor::TextEditor() : m_clickedOnMenu(false) {
    FontManager::init();
    m_menuFont = new Font(m_menuFontName, m_menuFontSize);

    m_textBox = new TextBox(m_defaultWidth, m_defaultHeight, m_textFontName, m_defaultThemeName);
    m_textBox->setWidth(500.0f);
}

TextEditor::~TextEditor() {
    delete m_textBox;
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
    if (!m_clickedOnMenu)
        handleMouseInput();

    // Draw the text box
    m_textBox->draw();
    ImGui::End();
}

void TextEditor::drawMenu() {
    ImGui::PushFont(m_menuFont->getFont());

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            m_clickedOnMenu = true;

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
        } else {
            m_clickedOnMenu = false;
        }

        if (ImGui::BeginMenu("Edit")) {
            m_clickedOnMenu = true;
            if (ImGui::MenuItem("Undo", "Ctrl+Z", false, !m_textBox->isUndoEmpty())) {
                m_textBox->undo();
            }
            if (ImGui::MenuItem("Redo", "Ctrl+Y", false, !m_textBox->isRedoEmpty())) {
                m_textBox->redo();
            }
            if (ImGui::MenuItem("Cut", "Ctrl+X", false, m_textBox->isSelectionActive())) {
                m_textBox->cut();
            }
            if (ImGui::MenuItem("Copy", "Ctrl+C", false, m_textBox->isSelectionActive())) {
                m_textBox->copy();
            }
            if (ImGui::MenuItem("Paste", "Ctrl+V")) {
                m_textBox->paste();
            }

            ImGui::EndMenu();
        } else {
            m_clickedOnMenu = false;
        }

        if (ImGui::BeginMenu("Theme")) {
            m_clickedOnMenu = true;

            if (ImGui::MenuItem("Light", "", false, m_textBox->getTheme() != ThemeManager::getTheme("Light"))) {
                m_textBox->setTheme(ThemeManager::getTheme("Light"));
            }
            if (ImGui::MenuItem("Dark", "", false, m_textBox->getTheme() != ThemeManager::getTheme("Dark"))) {
                m_textBox->setTheme(ThemeManager::getTheme("Dark"));
            }

            ImGui::EndMenu();
        } else {
            m_clickedOnMenu = false;
        }

        ImGui::EndMainMenuBar();
    }

    ImGui::PopFont();
}

void TextEditor::handleKeyboardInput() {
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
            m_textBox->enterChar('\n');
        } else if (isKeyPressed(ImGuiKey_Tab)) {
            m_textBox->tab(shift);
        } else if (isKeyPressed(ImGuiKey_Backspace)) {
            m_textBox->backspace();
        } else if (shift && isKeyPressed(ImGuiKey_Delete)) {
            m_textBox->deleteLine();
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
        } else if (ctrl && isKeyPressed(ImGuiKey_Z)) {
            m_textBox->undo();
        } else if (ctrl && isKeyPressed(ImGuiKey_Y)) {
            m_textBox->redo();
        } else if (ctrl && isKeyPressed(ImGuiKey_N, false)) {
            newFile();
        } else if (ctrl && isKeyPressed(ImGuiKey_O, false)) {
            open();
        } else if (ctrl && shift && isKeyPressed(ImGuiKey_S, false)) {
            saveAs();
        } else if (ctrl && isKeyPressed(ImGuiKey_S, false)) {
            save();
        }

        if (!io.InputQueueCharacters.empty()) {
            for (ImWchar c : io.InputQueueCharacters) {
                char ch = (char) c;

                if (std::isprint(ch)) {
                    std::cerr << "Entering char: " << ch << std::endl;
                    m_textBox->enterChar(ch);
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
        if (isInsideHorizontalScrollbar(position)) {
            m_textBox->horizontalBarClick(position);
        } else if (isInsideVerticalScrollbar(position)) {
            m_textBox->verticalBarClick(position);
        } if (isInsideTextBox(position)) {
            m_textBox->moveCursorToMousePosition(position);
        }
    } else if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        auto delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
        auto previousPosition = ImVec2(position.x - delta.x, position.y - delta.y);

        if (isInsideHorizontalScrollbar(position) && isInsideHorizontalScrollbar(previousPosition)) {
            m_textBox->horizontalScroll(position, delta);
        } else if (isInsideVerticalScrollbar(position) && isInsideVerticalScrollbar(previousPosition)) {
            m_textBox->verticalScroll(position, delta);
        } else if (isInsideTextBox(position) && isInsideTextBox(previousPosition)) {
            m_textBox->setMouseSelection(position, delta);
        }
    } else if (mouseWheel != 0.0f) {
        m_textBox->mouseWheelScroll(shift, mouseWheel);
    }
}

bool TextEditor::isInsideTextBox(ImVec2& mousePosition) {
    auto topLeft = ImGui::GetCursorScreenPos();
    auto bottomRight = ImVec2(topLeft.x + m_textBox->getWidth(), topLeft.y + m_textBox->getHeight());
    auto rect = MyRectangle(topLeft, bottomRight);

    return MyRectangle::isInsideRectangle(rect, mousePosition);
}

bool TextEditor::isInsideHorizontalScrollbar(ImVec2 &mousePosition) {
    return MyRectangle::isInsideRectangle(m_textBox->getHScrollbarRect(), mousePosition);
}

bool TextEditor::isInsideVerticalScrollbar(ImVec2 &mousePosition) {
    return MyRectangle::isInsideRectangle(m_textBox->getVScrollbarRect(), mousePosition);
}

bool TextEditor::isKeyPressed(ImGuiKey&& key, bool repeat) {
    return ImGui::IsKeyPressed(ImGui::GetKeyIndex(key), repeat);
}

void TextEditor::newFile() {
    auto id = handleFileNotSaved();
    if (id == IDCANCEL)
        return;

    m_textBox->newFile();
}

void TextEditor::open() {
    auto id = handleFileNotSaved();
    if (id == IDCANCEL)
        return;

    auto path = openFileDialog();

    if (!path.empty())
        m_textBox->open(path);
}

void TextEditor::save() {
    if (m_textBox->getFile() == nullptr)
        saveAs();
    else
        m_textBox->save();
}

void TextEditor::saveAs() {
    auto path = saveFileDialog();

    if (!path.empty()) {
        if (path.find_last_of('.') == std::string::npos)
            path += ".txt";
        m_textBox->saveAs(path);
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
           << (m_textBox->getFile() == nullptr ? "Untitled" : m_textBox->getFile()->getName())
           << "?";


    auto msgboxID = MessageBox(
            nullptr,
            stream.str().c_str(),
            reinterpret_cast<LPCSTR>(L"Text editor"),
            MB_YESNOCANCEL
    );

    return msgboxID;
}

int TextEditor::handleFileNotSaved() {
    if (m_textBox->isDirty()) {
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


