//
// Created by bbard on 6/30/2024.
//

#include "TextEditor.h"

TextEditor::TextEditor() : m_clickedOnMenu(false) {
    FontManager::init();
    m_menuFont = new Font(m_menuFontName, m_menuFontSize);

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
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);

    ImGui::Begin("TextEditor", 0, m_flags);
    drawMenu();

    m_inputHandler->handleKeyboardInput();

    if (!m_clickedOnMenu)
        m_inputHandler->handleMouseInput();

    m_textBox->draw();
    ImGui::End();
}

void TextEditor::drawMenu() {
    ImGui::PushFont(m_menuFont->getFont());

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            m_clickedOnMenu = true;

            if (ImGui::MenuItem("New", "Ctrl+N")) {
                m_textBox->newFile();
            }
            if (ImGui::MenuItem("Open...", "Ctrl+O")) {
                auto path = openFileDialog();
                auto opened = m_textBox->open(path);
            }
            if (ImGui::MenuItem("Save", "Ctrl+S")) {

            }
            if (ImGui::MenuItem("Save as...", "Ctrl+Shift+S")) {

            }

            ImGui::EndMenu();
        } else {
            m_clickedOnMenu = false;
        }

        if (ImGui::BeginMenu("Edit")) {
            m_clickedOnMenu = true;

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

        ImGui::EndMainMenuBar();
    }

    ImGui::PopFont();
}

std::string TextEditor::openFileDialog() {
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
                                      COINIT_DISABLE_OLE1DDE);

    PWSTR pszFilePath;

    if (SUCCEEDED(hr))
    {
        IFileOpenDialog *pFileOpen;

        // Create the FileOpenDialog object.
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
                              IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));

        if (SUCCEEDED(hr))
        {
            // Show the Open dialog box.
            hr = pFileOpen->Show(NULL);

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
            }
            pFileOpen->Release();
        }
        CoUninitialize();
    }

    std::wstringstream stream;
    stream << pszFilePath;
    const std::wstring wStr = stream.str();

    size_t len = wcstombs(nullptr, wStr.c_str(), 0) + 1;

    char* result = new char[len];
    wcstombs(result, wStr.c_str(), len);

    return {result};
}




