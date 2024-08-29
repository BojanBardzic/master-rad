//
// Created by bbard on 6/19/2024.
//

#include "TextBox.h"
#include <utility>

TextBox::TextBox(float width, float height, const std::string& fontName, const ThemeName theme) : m_dirty(false), m_file(nullptr) {

    m_pieceTableInstance = new PieceTableInstance();
    m_lineBuffer = new LineBuffer(m_pieceTableInstance);
    m_cursor = new Cursor(m_lineBuffer);
    m_selection = new Selection(m_lineBuffer);
    m_writeSelection = new Selection(m_lineBuffer);
    m_font = new Font(fontName);
    m_statusBarFont = new Font("Segoe UI", 19.0f);
    m_scroll = new Scroll(m_lineBuffer, m_cursor, m_font);
    m_theme = ThemeManager::getTheme(theme);

    m_width = width - m_bottomRightMargin.x - m_topLeftMargin.x;
    m_height = height - m_bottomRightMargin.y - m_topLeftMargin.y;

    // Fixme: Just a test input, will be removed later.
    m_pieceTableInstance->getInstance().insert("Hello World! World world world world world world world world world world world world\n\n\nHello There!\nSomething", 0);
    m_lineBuffer->getLines();
}

TextBox::~TextBox() {
    delete m_scroll;
    delete m_font;
    delete m_statusBarFont;
    delete m_selection;
    delete m_writeSelection;
    delete m_cursor;
    delete m_lineBuffer;
    delete m_pieceTableInstance;
}

// Draws the textBox based on PieceTable data.
void TextBox::draw() {
    if(!m_scroll->isInit())
        m_scroll->init(m_width, m_height);

    auto cursorScreenPosition = getTopLeft();

    // We track our current position as we draw the lines
    auto currentPosition = cursorScreenPosition;

    updateTextBoxSize();

    // Apply the current font
    ImGui::PushFont(m_font->getFont());

    auto lineHeight = ImGui::GetFontSize();
    auto linesSize = m_lineBuffer->getLinesSize();

    for (size_t i=0; i<linesSize; ++i) {
        // Get the line
        auto line = m_lineBuffer->lineAt(i);

        // Draw background rectangle
        drawRectangle(currentPosition, lineHeight);

        // Add a clip rectangle
        auto bottomRight = ImVec2(currentPosition.x + m_width, std::min(currentPosition.y + lineHeight + 2.0f, cursorScreenPosition.y + m_height));
        ImGui::GetWindowDrawList()->PushClipRect(cursorScreenPosition, bottomRight);

        auto xScroll = m_scroll->getXScroll();
        auto yScroll = m_scroll->getYScroll();

        auto textPosition = ImVec2(currentPosition.x - xScroll, currentPosition.y - yScroll);

        // Draw the selection if it's active
        drawSelection(m_writeSelection, textPosition, line, i, ThemeColor::WriteSelectColor);
        drawSelection(m_selection, textPosition, line, i, ThemeColor::SelectColor);

        // Draw the text
        drawText(textPosition, line, i);

        // Deactivate clip rectangle
        ImGui::GetWindowDrawList()->PopClipRect();

        // Update the current position
        currentPosition.y += ImGui::GetFontSize();
    }

    auto yOffset = currentPosition.y - cursorScreenPosition.y;

    // If we haven't filled the entire text box we draw the rest of the background
    if (yOffset < m_height) {
      ImGui::GetWindowDrawList()->AddRectFilled(currentPosition, getBottomRight(), m_theme->getColor(ThemeColor::BackgroundColor));
    }

    drawCursor();
    drawScrollBars();
    drawStatusBar();

    ImGui::PopFont();
}

// Enters single character in the pieceTable and updates the state of the text box
void TextBox::enterChar(char c) {
    updateUndoRedo();
    deleteSelection();

    m_pieceTableInstance->getInstance().flushDeleteBuffer();

    auto initialized = insertCharToPieceTable(c);

    if (initialized)
        m_cursor->recordCursorPosition();

    updateStateForTextChange(true, 1);

    m_cursor->moveRight();
    updateStateForCursorMovement();
}

// Enters a text in the piece table and updates the state of the text box
void TextBox::enterText(std::string str) {
    updateUndoRedo();
    deleteSelection();

    m_pieceTableInstance->getInstance().flushInsertBuffer();
    m_pieceTableInstance->getInstance().flushDeleteBuffer();
    m_cursor->recordCursorPosition();

    auto size = str.size();
    auto coords = m_cursor->getCoords();
    size_t index = m_lineBuffer->textCoordinatesToBufferIndex(coords);
    m_pieceTableInstance->getInstance().insert(std::move(str), index);

    updateStateForTextChange(true, size);

    auto newCoords = m_lineBuffer->bufferIndexToTextCoordinates(index + size);
    m_cursor->setCoords(newCoords);

    updateStateForCursorMovement();
}

// Preforms a backspace operation on the piece table and updates the state of the text box
void TextBox::backspace() {
    auto deleted = deleteSelection();

    if (deleted) {
        return;
    }

    auto coords = m_cursor->getCoords();
    size_t index = m_lineBuffer->textCoordinatesToBufferIndex(coords);
    if (index != 0) {
        updateUndoRedo();
        m_pieceTableInstance->getInstance().flushInsertBuffer();
        auto initialized = m_pieceTableInstance->getInstance().backspace(index);

        if (initialized)
            m_cursor->recordCursorPosition();

        updateStateForTextChange(false, 1);

        m_cursor->moveLeft();

        updateStateForCursorMovement();
    }
}

// Handles all the cases for the tab and tab + shift commands
void TextBox::tab(bool shift) {
    updateUndoRedo();

    // Event flags
    bool changed = false;
    bool cursorMovedLeft = false;
    bool cursorMovedRight = false;

    // Flush the buffers
    m_pieceTableInstance->getInstance().flushInsertBuffer();
    m_pieceTableInstance->getInstance().flushDeleteBuffer();

    // If there is an active selection and its multiline
    if (m_selection->isActive() && m_selection->getStart().m_row < m_selection->getEnd().m_row) {
        if (shift) {
            changed = deleteTabFromSelectedRows();
            if (changed) {
                cursorMovedLeft = true;
                m_selection->moveEndLeft();
            }
        } else {
            changed = addTabToSelectedRows();
            cursorMovedRight = true;
            m_selection->moveEndRight();
        }
    } else {
        if (shift) {
            changed = reverseTab();
            if (changed) {
                m_cursor->recordCursorPosition();
                cursorMovedLeft = true;
            }
        } else {
            deleteSelection();
            insertCharToPieceTable('\t');

            m_cursor->recordCursorPosition();
            cursorMovedRight = true;
            changed = true;
        }
    }

    if (changed)
        updateStateForTextChange(!cursorMovedLeft, 1);
    if (cursorMovedLeft)
        m_cursor->moveLeft();
    if (cursorMovedRight)
        m_cursor->moveRight();
    if (cursorMovedLeft || cursorMovedRight)
        updateStateForCursorMovement();
}

// Deletes the char right of the cursor if there is one
void TextBox::deleteChar() {
    auto deleted = deleteSelection();

    if (deleted) {
        return;
    }

    auto coords = m_cursor->getCoords();
    size_t index = m_lineBuffer->textCoordinatesToBufferIndex(coords);
    if (index != m_lineBuffer->getCharSize()) {
        updateUndoRedo();
        m_pieceTableInstance->getInstance().flushInsertBuffer();
        auto initialized = m_pieceTableInstance->getInstance().charDelete(index);

        if (initialized)
            m_cursor->recordCursorPosition();

        updateStateForTextChange(false, 1);
    }
}

void TextBox::deleteLine() {
    updateUndoRedo();
    m_pieceTableInstance->getInstance().flushInsertBuffer();
    m_pieceTableInstance->getInstance().flushDeleteBuffer();

    if (m_selection->isActive()) {
        deleteSelection();
        return;
    }

    auto row = m_cursor->getRow();
    auto line = m_lineBuffer->lineAt(row-1);
    auto index = m_lineBuffer->textCoordinatesToBufferIndex({row, 1});
    auto offset = line.size();

    if (row != m_lineBuffer->getLinesSize())
        offset++;

    m_cursor->recordCursorPosition();
    m_pieceTableInstance->getInstance().deleteText(index, index+offset);
    updateStateForTextChange(false, line.size());

    m_cursor->setCoords({std::min(row, m_lineBuffer->getLinesSize()), 1});
    updateStateForCursorMovement();
}

void TextBox::selectAll() {
    m_cursor->moveToEndOfFile();
    auto newCoords = m_cursor->getCoords();
    m_selection->selectAll(newCoords);
    updateStateForSelectionChange();
}

void TextBox::activateWriteSelection() {
    if (!m_selection->isActive())
        return;

    m_writeSelection->setActive(true);
    m_writeSelection->setStart(m_selection->getStart());
    m_writeSelection->setEnd(m_selection->getEnd());

    m_selection->setActive(false);
}

void TextBox::deactivateWriteSelection() {
    if (m_writeSelection->isActive()) {
        m_selection->setActive(false);
        m_writeSelection->setActive(false);
    }
}

void TextBox::cut() {
    if (m_selection->isActive()) {
        copySelectionToClipboard();
        deleteSelection();
    }
}

void TextBox::copy() {
    if (m_selection->isActive()) {
        copySelectionToClipboard();
    }
}

void TextBox::paste() {
    auto text = std::string(ImGui::GetClipboardText());
    if (!text.empty())
        enterText(text);
}

void TextBox::undo() {
    m_writeSelection->setActive(false);
    m_pieceTableInstance->getInstance().flushInsertBuffer();
    m_pieceTableInstance->getInstance().flushDeleteBuffer();
    m_pieceTableInstance->getInstance().undo();
    m_cursor->cursorUndo();
    updateStateForTextChange(true, 0);
    updateStateForCursorMovement();
}

void TextBox::redo() {
    m_writeSelection->setActive(false);
    m_pieceTableInstance->getInstance().redo();
    m_cursor->cursorRedo();
    updateStateForTextChange(true, 0);
    updateStateForCursorMovement();
}

void TextBox::newFile() {
    m_pieceTableInstance->newFile();

    m_lineBuffer->getLines();
    m_cursor->clearUndoAndRedoStacks();
    m_cursor->setCoords({1, 1});
    m_scroll->updateScroll(m_width, m_height);
    m_scroll->updateMaxScroll(m_width, m_height);
}

bool TextBox::open(std::string& filePath) {
    std::string buffer;
    // Try to read from the file path
    auto success = readFromFile(buffer, filePath);
    if (!success)
        return false;

    // If the read was successful update the filePath and pass the buffer contents to the piece table
    m_file = new File(filePath);
    m_lineBuffer->setLanguageMode(File::getModeForExtension(m_file->getExtension()));
    m_pieceTableInstance->open(buffer);

    // Update the state of the text box
    m_lineBuffer->getLines();
    m_cursor->clearUndoAndRedoStacks();
    m_cursor->setCoords({1, 1});
    m_scroll->updateScroll(m_width, m_height);
    m_scroll->updateMaxScroll(m_width, m_height);

    return true;
}

bool TextBox::save() {
    m_dirty = false;

    m_pieceTableInstance->getInstance().flushInsertBuffer();
    m_pieceTableInstance->getInstance().flushDeleteBuffer();

    return saveToFile();
}

bool TextBox::saveAs(std::string &filePath) {
    auto oldFile = m_file;
    m_file = new File(filePath);
    m_lineBuffer->setLanguageMode(File::getModeForExtension(m_file->getExtension()));
    delete oldFile;

    return save();
}

void TextBox::moveCursorRight(bool shift) {

    auto oldCoords = m_cursor->getCoords();

    m_cursor->moveRight();
    updateStateForCursorMovement();

    auto newCoords = m_cursor->getCoords();

    if (shift) {
        m_selection->update(oldCoords, newCoords);
        updateStateForSelectionChange();
    } else {
        if (m_selection->isActive()) {
            m_cursor->setCoords(m_selection->getEnd());
        }
        m_selection->setActive(false);
    }
}

void TextBox::moveCursorLeft(bool shift) {
    auto oldCoords = m_cursor->getCoords();

    m_cursor->moveLeft();
    updateStateForCursorMovement();

    auto newCoords = m_cursor->getCoords();

    if (shift) {
        m_selection->update(oldCoords, newCoords);
        updateStateForSelectionChange();
    } else {
        if (m_selection->isActive()) {
            m_cursor->setCoords(m_selection->getStart());
        }
        m_selection->setActive(false);
    }
}

void TextBox::moveCursorUp(bool shift) {
    auto oldCoords = m_cursor->getCoords();

    m_cursor->moveUp();
    updateStateForCursorMovement();

    auto newCoords = m_cursor->getCoords();

    if (shift) {
        m_selection->update(oldCoords, newCoords);
        updateStateForSelectionChange();
    } else {
        m_selection->setActive(false);
    }
}

void TextBox::moveCursorDown(bool shift) {
    auto oldCoords = m_cursor->getCoords();

    m_cursor->moveDown();
    updateStateForCursorMovement();

    auto newCoords = m_cursor->getCoords();

    if (shift) {
        m_selection->update(oldCoords, newCoords);
        updateStateForSelectionChange();
    } else {
        m_selection->setActive(false);
    }
}

void TextBox::moveCursorToBeginning(bool shift) {
    auto oldCoords = m_cursor->getCoords();

    m_cursor->moveToBeginning();
    m_scroll->updateXScroll(m_width);

    auto newCoords = m_cursor->getCoords();

    if (shift) {
        m_selection->update(oldCoords, newCoords);
        updateStateForSelectionChange();
    } else {
        m_selection->setActive(false);
    }
}

void TextBox::moveCursorToEnd(bool shift) {
    auto oldCoords = m_cursor->getCoords();

    m_cursor->moveToEnd();
    m_scroll->updateXScroll(m_width);

    auto newCoords = m_cursor->getCoords();

    if (shift) {
        m_selection->update(oldCoords, newCoords);
        updateStateForSelectionChange();
    } else {
        m_selection->setActive(false);
    }
}

void TextBox::moveCursorToMousePosition(ImVec2& mousePosition) {
    m_selection->setActive(false);
    auto coords = mousePositionToTextCoordinates(mousePosition);
    m_cursor->setCoords(coords);
    updateStateForCursorMovement();
}

void TextBox::setMouseSelection(ImVec2& endPosition, ImVec2& delta) {
    const ImVec2 endPositionCorrected = {endPosition.x + m_scroll->getXScroll(), endPosition.y + m_scroll->getYScroll() };
    const ImVec2 startPositionCorrected = {endPositionCorrected.x - delta.x, endPositionCorrected.y - delta.y};


    auto startPositionCoords = mousePositionToTextCoordinates(startPositionCorrected);
    auto endPositionCoords = mousePositionToTextCoordinates(endPositionCorrected);

    if (startPositionCoords > endPositionCoords) {
        auto tmp = startPositionCoords;
        startPositionCoords = endPositionCoords;
        endPositionCoords = tmp;
    }

    m_selection->setStart(startPositionCoords);
    m_selection->setEnd(endPositionCoords);
    m_selection->setActive(true);
    m_cursor->setCoords(endPositionCoords);

    updateStateForSelectionChange();
    updateStateForCursorMovement();
}

void TextBox::mouseWheelScroll(bool shift, float &mouseWheel) {
    if (shift)
        m_scroll->mouseWheelXScroll(mouseWheel);
    else
        m_scroll->mouseWheelYScroll(mouseWheel);
}

void TextBox::horizontalScroll(ImVec2 &mousePosition, ImVec2 &delta) {
    ImVec2 startPosition = ImVec2(mousePosition.x - delta.x, mousePosition.y - delta.y);

    if (m_scroll->getMaxXScroll() != 0.0f && MyRectangle::isInsideRectangle(m_scroll->getHScrollSelectRect(), startPosition)) {
        auto hScrollRect = m_scroll->getHScrollbarRect();
        auto hSelectRect = m_scroll->getHScrollSelectRect();

        auto hScrollWidth = hScrollRect.getBottomRight().x - hScrollRect.getTopLeft().x;
        auto hSelectWidth = hSelectRect.getBottomRight().x - hSelectRect.getTopLeft().x;
        auto width = hScrollWidth - hSelectWidth;

        std::cerr << "Delta: " <<  delta.x << std::endl;
        std::cerr << "Width: " << width << std::endl;

        auto scrollIncrement = 0.5f * std::abs(delta.x) / width;
        std::cerr << "scrollIncrement: " << scrollIncrement << std::endl;
        auto maxXScroll = m_scroll->getMaxXScroll();

        if (delta.x > 0.0f)
            m_scroll->setXScroll(std::min(m_scroll->getXScroll() + scrollIncrement * maxXScroll, maxXScroll));
        else
            m_scroll->setXScroll(std::max(m_scroll->getXScroll() - scrollIncrement * maxXScroll, 0.0f));
    }
}

void TextBox::verticalScroll(ImVec2& mousePosition, ImVec2& delta) {
    ImVec2 startPosition = ImVec2(mousePosition.x - delta.x, mousePosition.y - delta.y);

    if (m_scroll->getMaxYScroll() != 0.0f && MyRectangle::isInsideRectangle(m_scroll->getVScrollSelectRect(), startPosition)) {
        auto vScrollRect = m_scroll->getVScrollbarRect();
        auto vSelectRect = m_scroll->getVScrollSelectRect();

        auto vScrollHeight = vScrollRect.getBottomRight().y - vScrollRect.getTopLeft().y;
        auto vSelectHeight = vSelectRect.getBottomRight().y - vSelectRect.getTopLeft().y;
        auto height = vScrollHeight - vSelectHeight;

        auto scrollIncrement = std::abs(delta.y) / height;
        auto maxYScroll = m_scroll->getMaxYScroll();

        if (delta.y > 0.0f)
            m_scroll->setYScroll(std::min(m_scroll->getYScroll() + scrollIncrement*maxYScroll, maxYScroll));
        else
            m_scroll->setYScroll(std::max(m_scroll->getYScroll() - scrollIncrement*maxYScroll, 0.0f));
    }
}

void TextBox::horizontalBarClick(ImVec2 &mousePosition) {
    auto offset = 30.0f;

    if (MyRectangle::isRightOfRectangle(m_scroll->getHScrollSelectRect(), mousePosition)) {
        m_scroll->setXScroll(std::min(m_scroll->getXScroll() + offset, m_scroll->getMaxXScroll()));
    } else if (MyRectangle::isLeftOfRectangle(m_scroll->getHScrollSelectRect(), mousePosition)) {
        m_scroll->setXScroll(std::max(m_scroll->getXScroll() - offset, 0.0f));
    }
}

void TextBox::verticalBarClick(ImVec2 &mousePosition) {
    auto offset = 30.0f;

    if (MyRectangle::isBelowRectangle(m_scroll->getVScrollSelectRect(), mousePosition)) {
        m_scroll->setYScroll(std::min(m_scroll->getYScroll() + offset, m_scroll->getMaxYScroll()));
    } else if (MyRectangle::isAboveRectangle(m_scroll->getVScrollSelectRect(), mousePosition)) {
        m_scroll->setYScroll(std::max(m_scroll->getYScroll() - offset, 0.0f));
    }
}

void TextBox::increaseFontSize() {
    m_font->increaseSize();
    m_scroll->updateMaxScroll(m_width, m_height);
}

void TextBox::decreaseFontSize() {
    m_font->decreaseSize();
    m_scroll->updateMaxScroll(m_width, m_height);
}

float TextBox::getWidth() const { return m_width; }

float TextBox::getHeight() const { return m_height; }

ImVec2 TextBox::getTopLeft() const {
    auto screenPosition = ImGui::GetCursorScreenPos();
    return {screenPosition.x + m_topLeftMargin.x, screenPosition.y + m_topLeftMargin.y};
}

ImVec2 TextBox::getBottomRight() const {
    auto topLeft = getTopLeft();
    return {topLeft.x + m_width, topLeft.y + m_height};
}

const MyRectangle &TextBox::getHScrollbarRect() const { return m_scroll->getHScrollbarRect(); }

const MyRectangle &TextBox::getVScrollbarRect() const { return m_scroll->getVScrollbarRect(); }

float TextBox::getScrollbarSize() const { return m_scrollbarSize; }

Cursor *TextBox::getCursor() const { return m_cursor; }

Theme* TextBox::getTheme() const { return m_theme; }

File* TextBox::getFile() const { return m_file; }

bool TextBox::isSelectionActive() const { return m_selection->isActive(); }

bool TextBox::isWriteSelectionActive() const { return m_writeSelection->isActive(); }

bool TextBox::isDirty() const { return m_dirty; }

bool TextBox::isUndoEmpty() const { return m_pieceTableInstance->getInstance().isUndoEmpty(); }

bool TextBox::isRedoEmpty() const { return m_pieceTableInstance->getInstance().isRedoEmpty(); }

void TextBox::setWidth(float width) { m_width = width; }

void TextBox::setHeight(float height) { m_height = height; }

void TextBox::setTheme(Theme* theme) { m_theme = theme; }

// Inserts a char into the piece table at the current cursor position
// and returns whether the add buffer was initialized after being flushed
bool TextBox::insertCharToPieceTable(char c) {
    auto coords = m_cursor->getCoords();
    size_t index = m_lineBuffer->textCoordinatesToBufferIndex(coords);
    return m_pieceTableInstance->getInstance().insertChar(c, index);
}

// Deletes the text inside a selection if it's active.
bool TextBox::deleteSelection() {
    if (!m_selection->isActive())
        return false;

    updateUndoRedo();

    m_pieceTableInstance->getInstance().flushInsertBuffer();
    m_pieceTableInstance->getInstance().flushDeleteBuffer();

    m_cursor->recordCursorPosition();

    auto startIndex = m_lineBuffer->textCoordinatesToBufferIndex(m_selection->getStart());
    auto endIndex = m_lineBuffer->textCoordinatesToBufferIndex(m_selection->getEnd());
    m_pieceTableInstance->getInstance().deleteText(startIndex, endIndex);

    updateStateForTextChange(false, endIndex-startIndex);

    m_cursor->setCoords(m_selection->getStart());
    updateStateForCursorMovement();

    m_selection->setActive(false);

    return true;
}

void TextBox::copySelectionToClipboard() {
    auto selectionText = m_selection->getSelectionText();
    ImGui::SetClipboardText(selectionText.c_str());
}

TextCoordinates TextBox::mousePositionToTextCoordinates(const ImVec2 &mousePosition) {
    ImGui::PushFont(m_font->getFont());

    float rows = (mousePosition.y - getTopLeft().y) / ImGui::GetFontSize();
    int maxValue = m_lineBuffer->isEmpty() ? 1 : m_lineBuffer->getLinesSize() - 1;
    size_t row = std::min(maxValue, (int) rows);
    auto line = m_lineBuffer->lineAt(row);

    size_t column = 1;

    float xCoords = getTopLeft().x - 5.0f;
    size_t i = 0;
    while (i < line.size()) {
        auto advance = ImGui::GetFont()->GetCharAdvance(line.at(i));
        if (mousePosition.x < xCoords + advance) {
            break;
        }
        ++column;
        xCoords += advance;
        ++i;
    }

    ImGui::PopFont();

    return {row+1, column};
}

// Adds a tab character in front of every selected row
// Returns whether there were added tabs
bool TextBox::addTabToSelectedRows() {
    auto beginRow = m_selection->getStart().m_row;
    auto endRow = m_selection->getEnd().m_row;

    auto index = m_lineBuffer->textCoordinatesToBufferIndex({beginRow, 1});
    m_cursor->recordCursorPosition();
    m_pieceTableInstance->getInstance().insert("\t", index);

    for (size_t i=beginRow-1; i<endRow-1; ++i) {
        auto line = m_lineBuffer->lineAt(i);
        auto lineSize = line.empty() ? 0 : line.size() + 1;
        index += lineSize + 1;
        if (!m_lineBuffer->lineAt(i+1).empty()) {
            m_cursor->recordCursorPosition();
            m_pieceTableInstance->getInstance().insert("\t", index);
        }
    }

    return true;
}

// Deletes a tab from the selected rows
// Returns whether there were deleted tabs
bool TextBox::deleteTabFromSelectedRows() {
    auto beginRow = m_selection->getStart().m_row;
    auto endRow = m_selection->getEnd().m_row;

    bool deleted = false;

    auto index = m_lineBuffer->textCoordinatesToBufferIndex({beginRow, 1});
    if (m_lineBuffer->lineStarsWithTab(beginRow-1)) {
        deleted = true;
        m_cursor->recordCursorPosition();
        m_pieceTableInstance->getInstance().deleteText(index, index + 1);
        --index;
    }

    for (size_t i=beginRow-1; i<endRow-1; ++i) {
        auto line = m_lineBuffer->lineAt(i);
        index += line.size() + 1;


        if (m_lineBuffer->lineStarsWithTab(i+1)) {
            deleted = true;
            m_cursor->recordCursorPosition();
            m_pieceTableInstance->getInstance().deleteText(index, index+1);
            --index;
        }
    }

    return deleted;
}

// Removes a tab character from the beginning of the line if there is one
// Returns whether anything was deleted
bool TextBox::reverseTab() {
    auto row = m_cursor->getRow();
    auto line = m_lineBuffer->lineAt(row-1);
    if (!line.empty() && line[0] == '\t') {
        auto index = m_lineBuffer->textCoordinatesToBufferIndex({row, 1});
        m_pieceTableInstance->getInstance().deleteText(index, index+1);
        return true;
    }
    return false;
}

inline void TextBox::drawRectangle(ImVec2 currentPosition, float& lineHeight) {
    auto screenPosition = getTopLeft();
    auto topLeft = currentPosition;
    auto bottomRight = ImVec2(topLeft.x + m_width, topLeft.y + lineHeight);

    // Add a clip rectangle
    ImGui::GetWindowDrawList()->PushClipRect(screenPosition, ImVec2(screenPosition.x + m_width, screenPosition.y + m_height));
    // Draw the background rectangle
    ImGui::GetWindowDrawList()->AddRectFilled(topLeft, bottomRight, m_theme->getColor(ThemeColor::BackgroundColor));
    // Deactivate clip rectangle
    ImGui::GetWindowDrawList()->PopClipRect();
}

void TextBox::drawText(ImVec2 textPosition, const std::string &line, size_t index) {
    if (m_lineBuffer->getLanguageMode() == LanguageMode::PlainText) {
        ImGui::GetWindowDrawList()->AddText(textPosition, m_theme->getColor(ThemeColor::TextColor), line.c_str());
        return;
    }

    auto colorMap = m_lineBuffer->getColorMap(index);

    size_t start = 0;
    while (start < line.size()) {
        size_t length = 0;
        ThemeColor color = colorMap[start];

        while (start + length < line.size() && colorMap[start + length] == color) {
            ++length;
        }

        auto text = line.substr(start, length);
        ImGui::GetWindowDrawList()->AddText(textPosition, m_theme->getColor(color), text.c_str());
        textPosition.x += m_cursor->getXAdvance(text);
        start += length;
    }
}

void TextBox::drawSelection(Selection* selection, ImVec2 textPosition, std::string& line, size_t i, ThemeColor color) {
    if (!selection->isActive())
        return;

    auto overlap = selection->getIntersectionWithLine(i);

    // Draw the selection rectangle if it's active
    if (selection->isActive() && overlap != std::pair{0, 0}) {
        auto leftOverlap = overlap.first;
        auto rightOverlap = overlap.second;

        auto leftString = line.substr(0, leftOverlap);
        auto middleString = line.substr(leftOverlap, rightOverlap-leftOverlap);

        auto leftStringAdvance = m_cursor->getXAdvance(leftString);
        auto middleStringAdvance = m_cursor->getXAdvance(middleString);

        ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(textPosition.x + leftStringAdvance, textPosition.y),
                                                  ImVec2(textPosition.x + leftStringAdvance + middleStringAdvance, textPosition.y + ImGui::GetFontSize()),
                                                  m_theme->getColor(color));
    }
}

void TextBox::drawCursor() {
    if (!m_cursor->getShouldRender()) {
        m_cursor->updateShouldRender();
        return;
    }

    m_cursor->calculateWidth();

    auto topLeft = m_cursor->getCursorPosition(getTopLeft());
    topLeft.x -= m_scroll->getXScroll();
    topLeft.y -= m_scroll->getYScroll();

    auto bottomRight = ImVec2(topLeft.x + m_cursor->getWidth(), topLeft.y + ImGui::GetFontSize());

    ImGui::GetWindowDrawList()->AddRectFilled(topLeft, bottomRight, m_theme->getColor(ThemeColor::CursorColor));

    m_cursor->updateShouldRender();
}


// Draws both scrollbars
void TextBox::drawScrollBars() {
    drawHorizontalScrollbar();
    drawVerticalScrollbar();
    auto screenPosition = getTopLeft();
    auto topLeft = ImVec2(screenPosition.x + m_width, screenPosition.y + m_height);
    auto bottomRight = ImVec2(topLeft.x + m_scrollbarSize, topLeft.y + m_scrollbarSize);
    ImGui::GetWindowDrawList()->AddRectFilled(topLeft, bottomRight, m_theme->getColor(ThemeColor::ScrollbarPrimaryColor));
}

// Draws the entire horizontal scrollbar
void TextBox::drawHorizontalScrollbar() {
    updateHScrollbarRect();
    drawHScrollbarRect();

    auto maxXScroll = m_scroll->getMaxXScroll();

    if (maxXScroll != 0.0f) {
        updateHScrollSelectRect();
        drawRect(m_scroll->getHScrollSelectRect(), m_theme->getColor(ThemeColor::ScrollbarPrimaryColor));
    }
}

// Draws the entire vertical scrollbar
void TextBox::drawVerticalScrollbar() {
    updateVScrollbarRect();
    drawVScrollbarRect();

    auto maxYScroll = m_scroll->getMaxYScroll();

    if (maxYScroll != 0.0f) {
        updateVScrollSelectRect();
        drawRect(m_scroll->getVScrollSelectRect(), m_theme->getColor(ThemeColor::ScrollbarPrimaryColor));
    }
}

// Draw the horizontal scrollbar rectangle
void TextBox::drawHScrollbarRect() {
    auto rect = m_scroll->getHScrollbarRect();
    auto topLeft = rect.getTopLeft();
    auto bottomRight = rect.getBottomRight();

    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(topLeft.x - m_scrollbarSize, topLeft.y), ImVec2(bottomRight.x + m_scrollbarSize, bottomRight.y), m_theme->getColor(ThemeColor::ScrollbarPrimaryColor));
    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(topLeft.x, topLeft.y), ImVec2(bottomRight.x , bottomRight.y), m_theme->getColor(ThemeColor::ScrollbarSecondaryColor));
}

// Draw the vertical scrollbar rectangle
void TextBox::drawVScrollbarRect() {
    auto rect = m_scroll->getVScrollbarRect();
    auto topLeft = rect.getTopLeft();
    auto bottomRight = rect.getBottomRight();

    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(topLeft.x, topLeft.y - m_scrollbarSize), ImVec2(bottomRight.x, bottomRight.y + m_scrollbarSize), m_theme->getColor(ThemeColor::ScrollbarPrimaryColor));
    ImGui::GetWindowDrawList()->AddRectFilled(topLeft, bottomRight, m_theme->getColor(ThemeColor::ScrollbarSecondaryColor));
}

void TextBox::drawRect(const MyRectangle &rect, const ImColor& color) {
    ImGui::GetWindowDrawList()->AddRectFilled(rect.getTopLeft(), rect.getBottomRight(), color);
}

// Updates the horizontal scrollbar rectangle
void TextBox::updateHScrollbarRect() {
    auto screenPosition = getTopLeft();
    auto topLeft = ImVec2(screenPosition.x + m_scrollbarSize, screenPosition.y + m_height);
    auto bottomRight = ImVec2(topLeft.x + m_width - m_scrollbarSize, topLeft.y + m_scrollbarSize);
    m_scroll->setHScrollbarRect(MyRectangle(topLeft, bottomRight));
}

// Updates the vertical scrollbar rectangle
void TextBox::updateVScrollbarRect() {
    auto screenPosition = getTopLeft();
    auto topLeft = ImVec2(screenPosition.x + m_width, screenPosition.y + m_scrollbarSize);
    auto bottomRight = ImVec2(topLeft.x + m_scrollbarSize, topLeft.y + m_height - m_scrollbarSize);
    m_scroll->setVScrollbarRect(MyRectangle(topLeft, bottomRight));
}

// Updates the rectangle coordinates for the horizontal scroll select rectangle
void TextBox::updateHScrollSelectRect() {
    auto maxXScroll = m_scroll->getMaxXScroll();
    auto xScroll = m_scroll->getXScroll();
    auto percentVisible = m_width / (m_width + maxXScroll);
    auto scrollBarWidth = m_width - 2 * m_scrollbarSize;

    auto scrollSelectSize = std::max(scrollBarWidth*percentVisible, m_minScrollSelectSize);
    auto scrollBarOffset = (scrollBarWidth - scrollSelectSize) * (xScroll / maxXScroll);
    auto topLeft = m_scroll->getHScrollbarRect().getTopLeft();

    auto topLeftScrollSelect = ImVec2(topLeft.x + scrollBarOffset, topLeft.y);
    auto bottomRightScrollSelect = ImVec2(topLeftScrollSelect.x + scrollSelectSize + m_scrollbarSize, topLeftScrollSelect.y + m_scrollbarSize);

    m_scroll->setHScrollSelectRect(MyRectangle(topLeftScrollSelect, bottomRightScrollSelect));
}

// Updates the rectangle coordinates for the vertical scroll select rectangle
void TextBox::updateVScrollSelectRect() {
    auto maxYScroll = m_scroll->getMaxYScroll();
    auto yScroll = m_scroll->getYScroll();
    auto percentVisible = m_height / (m_height + maxYScroll);
    auto scrollBarHeight = m_height - 2 * m_scrollbarSize;

    auto scrollSelectSize = std::max(scrollBarHeight*percentVisible, m_minScrollSelectSize);
    auto scrollBarOffset = (scrollBarHeight - scrollSelectSize) * (yScroll / maxYScroll);
    auto topLeft = m_scroll->getVScrollbarRect().getTopLeft();

    auto topLeftScrollSelect = ImVec2(topLeft.x, topLeft.y + scrollBarOffset);
    auto bottomRightScrollSelect = ImVec2(topLeftScrollSelect.x + m_scrollbarSize, topLeftScrollSelect.y + scrollSelectSize + m_scrollbarSize);

    m_scroll->setVScrollSelectRect(MyRectangle(topLeftScrollSelect, bottomRightScrollSelect));
}

void TextBox::drawStatusBar() {
    float offset = 15.0f;

    ImGui::PushFont(m_statusBarFont->getFont());

    auto topLeft = getTopLeft();
    auto textPosition = ImVec2(topLeft.x, topLeft.y + m_height + offset + 5.0f);
    auto text = getStatusBarText();
    ImGui::GetWindowDrawList()->AddText(textPosition, ImColor(255, 255, 255), text.c_str());

    ImGui::PopFont();
}

std::string TextBox::getStatusBarText() {
    std::stringstream stream;
    stream << (m_file == nullptr ? "Untitled" : m_file->getName()) << (m_dirty ? "*" : " ") << " |  ";
    stream << (m_lineBuffer->getLanguageMode() == LanguageMode::PlainText ? "Plain text" : LanguageManager::getLanguage(m_lineBuffer->getLanguageMode())->getName()) << " |  ";
    stream << "Row: " << m_cursor->getRow() << " Column: " << m_cursor->getCol();
    return stream.str();
}

// Reads from the current file and writes the contents into buffer
bool TextBox::readFromFile(std::string& buffer, const std::string& filePath) {
    std::ifstream input(filePath);

    if (!input.is_open())
        return false;

    try {
        std::getline(input, buffer, '\0');
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return false;
    }

    input.close();
    return true;
}

// Saves the text box contents to the current file
bool TextBox::saveToFile() {
    std::string backupBuffer;
    readFromFile(backupBuffer, m_file->getPath());

    std::ofstream output(m_file->getPath());
    if (!output.is_open())
        return false;

    try {
        output << m_pieceTableInstance->getInstance();
        output << '\0';
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        output.close();

        std::ofstream backupOutput(m_file->getPath());
        backupOutput << backupBuffer;
        backupOutput << '\0';
        backupOutput.close();

        return false;
    }

    output.close();
    return true;
}

// Clears the undo and redo stacks if we are in a past state
void TextBox::updateUndoRedo() {
    if (!m_pieceTableInstance->getInstance().isRedoEmpty()) {
        m_pieceTableInstance->getInstance().clearUndoAndRedoStacks();
        m_cursor->clearUndoAndRedoStacks();
    }
}

// Updates the TextBox size to the size of the window.
void TextBox::updateTextBoxSize() {
    bool changedWidth = false;
    bool changedHeight = false;

    auto newWidth = ImGui::GetWindowWidth() - m_bottomRightMargin.x - m_topLeftMargin.x - ImGui::GetCursorScreenPos().x - m_scrollbarSize;
    auto newHeight = ImGui::GetWindowHeight() - m_bottomRightMargin.y - m_topLeftMargin.y - ImGui::GetCursorScreenPos().y - m_scrollbarSize;

    if (m_width != newWidth) {
        m_width = newWidth;
        changedWidth = true;
    }

    if (m_height != newHeight) {
        m_height = newHeight;
        changedHeight = true;
    }

    if (changedWidth)
        m_scroll->updateMaxXScroll(m_width);
    if (changedHeight)
        m_scroll->updateMaxYScroll(m_height);
}

void TextBox::updateStateForTextChange(bool isInsert, size_t size) {
    updateWriteSelection(isInsert, size);
    m_lineBuffer->getLines();
    m_scroll->updateMaxScroll(m_width, m_height);
    m_dirty = true;
}

void TextBox::updateWriteSelection(bool isInsert, size_t size) {
    if (!m_writeSelection->isActive() || size == 0)
        return;

    if (isInsert)
        m_writeSelection->moveEndRight(size);
    else
        m_writeSelection->moveEndLeft(size);
}

void TextBox::updateStateForCursorMovement() {
    if (m_writeSelection->isActive()) {
        m_cursor->clipCursor(m_writeSelection->getStart(), m_writeSelection->getEnd());
    }
    m_scroll->updateScroll(m_width, m_height);
}

void TextBox::updateStateForSelectionChange() {
    if (m_writeSelection->isActive() && m_selection->isActive())
        m_selection->clipSelection(m_writeSelection->getStart(), m_writeSelection->getEnd());
}



























