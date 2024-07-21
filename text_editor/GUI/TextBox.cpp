//
// Created by bbard on 6/19/2024.
//

#include "TextBox.h"
#include "ThemeManager.h"
#include <utility>

TextBox::TextBox(float width, float height, std::string fontName, std::string theme) : m_dirty(false), m_file(nullptr) {

    m_pieceTableInstance = new PieceTableInstance();
    m_lineBuffer = new LineBuffer(m_pieceTableInstance);
    m_cursor = new Cursor(m_lineBuffer);
    m_selection = new Selection(m_lineBuffer);
    m_font = new Font(fontName);
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
    delete m_selection;
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

        auto overlap = m_selection->getIntersectionWithLine(i);

        auto xScroll = m_scroll->getXScroll();
        auto yScroll = m_scroll->getYScroll();
        auto textPosition = ImVec2(currentPosition.x - xScroll, currentPosition.y - yScroll);

        if (m_selection->getActive() && overlap != std::pair{0, 0}) {
            auto leftOverlap = overlap.first;
            auto rightOverlap = overlap.second;

            auto leftString = line.substr(0, leftOverlap);
            auto middleString = line.substr(leftOverlap, rightOverlap-leftOverlap);
            auto rightString = line.substr(rightOverlap);

            if (!leftString.empty()) {
                ImGui::GetWindowDrawList()->AddText(textPosition, m_theme->getTextColor(), leftString.c_str());
                textPosition.x += m_cursor->getXAdvance(leftString);
            }

            auto secondStringAdvance = m_cursor->getXAdvance(middleString);
            ImGui::GetWindowDrawList()->AddRectFilled(textPosition, ImVec2(textPosition.x + secondStringAdvance, textPosition.y + lineHeight), m_theme->getSelectColor());
            ImGui::GetWindowDrawList()->AddText(textPosition, m_theme->getSelectTextColor(), middleString.c_str());
            textPosition.x += secondStringAdvance;


            if (!rightString.empty()) {
                ImGui::GetWindowDrawList()->AddText(textPosition, m_theme->getTextColor(), rightString.c_str());
            }

        } else {
            // Draw the line text
            ImGui::GetWindowDrawList()->AddText(textPosition, m_theme->getTextColor(), line.c_str());
        }

        // Deactivate clip rectangle
        ImGui::GetWindowDrawList()->PopClipRect();

        // Update the current position
        currentPosition.y += ImGui::GetFontSize();
    }

    auto yOffset = currentPosition.y - cursorScreenPosition.y;

    if (yOffset < m_height) {
      ImGui::GetWindowDrawList()->AddRectFilled(currentPosition, getBottomRight(), m_theme->getBackGroundColor());
    }

    if (m_cursor->getShouldRender())
        drawCursor();
    m_cursor->updateShouldRender();

    drawScrollBars();

    drawStatusBar();

    ImGui::PopFont();
}

// Enters single character in the pieceTable
void TextBox::enterChar(char c) {
    updateUndoRedo();
    m_dirty = true;
    deleteSelection();

    m_pieceTableInstance->getInstance().flushDeleteBuffer();

    auto coords = m_cursor->getCoords();
    size_t index = m_lineBuffer->textCoordinatesToBufferIndex(coords);

    auto initialized = m_pieceTableInstance->getInstance().insertChar(c, index);

    if (initialized)
        m_cursor->recordCursorPosition();

    m_lineBuffer->getLines();
    m_cursor->moveRight();
    m_scroll->updateScroll(m_width, m_height);
    m_scroll->updateMaxScroll(m_width, m_height);
}

// Enters a text in the piece table and updates the state of the text box
void TextBox::enterText(std::string str) {
    updateUndoRedo();
    m_dirty = true;
    deleteSelection();

    m_pieceTableInstance->getInstance().flushInsertBuffer();
    m_pieceTableInstance->getInstance().flushDeleteBuffer();
    m_cursor->recordCursorPosition();

    auto size = str.size();
    auto coords = m_cursor->getCoords();
    size_t index = m_lineBuffer->textCoordinatesToBufferIndex(coords);
    m_pieceTableInstance->getInstance().insert(std::move(str), index);
    m_lineBuffer->getLines();

    auto newCoords = m_lineBuffer->bufferIndexToTextCoordinates(index + size);
    m_cursor->setCoords(newCoords);

    m_scroll->updateScroll(m_width, m_height);
    m_scroll->updateMaxScroll(m_width, m_height);
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
        m_dirty = true;
        m_pieceTableInstance->getInstance().flushInsertBuffer();
        auto initialized = m_pieceTableInstance->getInstance().backspace(index);

        if (initialized)
            m_cursor->recordCursorPosition();

        m_cursor->moveLeft();
        m_lineBuffer->getLines();
        m_scroll->updateScroll(m_width, m_height);
        m_scroll->updateMaxScroll(m_width, m_height);
    }
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
        m_dirty = true;
        m_pieceTableInstance->getInstance().flushInsertBuffer();
        auto initialized = m_pieceTableInstance->getInstance().charDelete(index);

        if (initialized)
            m_cursor->recordCursorPosition();

        m_lineBuffer->getLines();
        m_scroll->updateMaxScroll(m_width, m_height);
    }
}

void TextBox::selectAll() {
    m_cursor->moveToEndOfFile();
    auto newCoords = m_cursor->getCoords();
    m_selection->selectAll(newCoords);
}

void TextBox::cut() {
    if (m_selection->getActive()) {
        copySelectionToClipboard();
        deleteSelection();
    }
}

void TextBox::copy() {
    if (m_selection->getActive()) {
        copySelectionToClipboard();
    }
}

void TextBox::paste() {
    auto text = std::string(ImGui::GetClipboardText());
    if (!text.empty())
        enterText(text);
}

void TextBox::undo() {
    m_pieceTableInstance->getInstance().flushInsertBuffer();
    m_pieceTableInstance->getInstance().flushDeleteBuffer();
    m_pieceTableInstance->getInstance().undo();
    m_cursor->cursorUndo();
    m_lineBuffer->getLines();
    m_scroll->updateScroll(m_width, m_height);
    m_scroll->updateMaxScroll(m_width, m_height);
}

void TextBox::redo() {
    m_pieceTableInstance->getInstance().redo();
    m_cursor->cursorRedo();
    m_lineBuffer->getLines();
    m_scroll->updateScroll(m_width, m_height);
    m_scroll->updateMaxScroll(m_width, m_height);
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
    std::ifstream inputFile(filePath);

    if (!inputFile.is_open()) {
        return false;
    }

    m_file = new File(filePath);
    m_pieceTableInstance->open(filePath);
    m_lineBuffer->getLines();
    m_cursor->clearUndoAndRedoStacks();
    m_cursor->setCoords({1, 1});
    m_scroll->updateScroll(m_width, m_height);
    m_scroll->updateMaxScroll(m_width, m_height);

    return true;
}

void TextBox::moveCursorRight(bool shift) {

    auto oldCoords = m_cursor->getCoords();

    m_cursor->moveRight();
    m_scroll->updateScroll(m_width, m_height);

    auto newCoords = m_cursor->getCoords();

    if (shift) {
        m_selection->update(oldCoords, newCoords);
    } else {
        if (m_selection->getActive()) {
            m_cursor->setCoords(m_selection->getEnd());
        }
        m_selection->setActive(false);
    }
}

void TextBox::moveCursorLeft(bool shift) {
    auto oldCoords = m_cursor->getCoords();

    m_cursor->moveLeft();
    m_scroll->updateScroll(m_width, m_height);

    auto newCoords = m_cursor->getCoords();

    if (shift) {
        m_selection->update(oldCoords, newCoords);
    } else {
        if (m_selection->getActive()) {
            m_cursor->setCoords(m_selection->getStart());
        }
        m_selection->setActive(false);
    }
}

void TextBox::moveCursorUp(bool shift) {
    auto oldCoords = m_cursor->getCoords();

    m_cursor->moveUp();
    m_scroll->updateScroll(m_width, m_height);

    auto newCoords = m_cursor->getCoords();

    if (shift) {
        m_selection->update(oldCoords, newCoords);
    } else {
        m_selection->setActive(false);
    }
}

void TextBox::moveCursorDown(bool shift) {
    auto oldCoords = m_cursor->getCoords();

    m_cursor->moveDown();
    m_scroll->updateScroll(m_width, m_height);

    auto newCoords = m_cursor->getCoords();

    if (shift) {
        m_selection->update(oldCoords, newCoords);
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
    } else {
        m_selection->setActive(false);
    }
}

void TextBox::moveCursorToMousePosition(ImVec2& mousePosition) {
    m_selection->setActive(false);
    auto coords = mousePositionToTextCoordinates(mousePosition);
    m_cursor->setCoords(coords);
}

void TextBox::setMouseSelection(ImVec2& endPosition, ImVec2& delta) {
    const ImVec2 startPosition = {endPosition.x - delta.x, endPosition.y - delta.y};
    auto startPositionCoords = mousePositionToTextCoordinates(startPosition);
    auto endPositionCoords = mousePositionToTextCoordinates(endPosition);

    if (startPositionCoords > endPositionCoords) {
        auto tmp = startPositionCoords;
        startPositionCoords = endPositionCoords;
        endPositionCoords = tmp;
    }

    m_selection->setStart(startPositionCoords);
    m_selection->setEnd(endPositionCoords);
    m_selection->setActive(true);
    m_cursor->setCoords(endPositionCoords);
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

bool TextBox::isSelectionActive() const { return m_selection->getActive(); }

void TextBox::setWidth(float width) { m_width = width; }

void TextBox::setHeight(float height) { m_height = height; }

// Deletes the text inside a selection if it's active.
bool TextBox::deleteSelection() {
    if (!m_selection->getActive())
        return false;

    updateUndoRedo();
    m_dirty = true;

    m_pieceTableInstance->getInstance().flushInsertBuffer();
    m_pieceTableInstance->getInstance().flushDeleteBuffer();

    m_cursor->recordCursorPosition();

    auto startIndex = m_lineBuffer->textCoordinatesToBufferIndex(m_selection->getStart());
    auto endIndex = m_lineBuffer->textCoordinatesToBufferIndex(m_selection->getEnd());
    m_pieceTableInstance->getInstance().deleteText(startIndex, endIndex);

    m_lineBuffer->getLines();
    m_cursor->setCoords(m_selection->getStart());
    m_scroll->updateScroll(m_width, m_height);
    m_scroll->updateMaxScroll(m_width, m_height);
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

    return TextCoordinates(row+1, column);
}

inline void TextBox::drawRectangle(ImVec2 currentPosition, float& lineHeight) {
    auto screenPosition = getTopLeft();
    auto topLeft = currentPosition;
    auto bottomRight = ImVec2(topLeft.x + m_width, topLeft.y + lineHeight);

    // Add a clip rectangle
    ImGui::GetWindowDrawList()->PushClipRect(screenPosition, ImVec2(screenPosition.x + m_width, screenPosition.y + m_height));
    // Draw the background rectangle
    ImGui::GetWindowDrawList()->AddRectFilled(topLeft, bottomRight, m_theme->getBackGroundColor());
    // Deactivate clip rectangle
    ImGui::GetWindowDrawList()->PopClipRect();
}

void TextBox::drawCursor() {
    m_cursor->calculateWidth();

    auto topLeft = m_cursor->getCursorPosition(getTopLeft());
    topLeft.x -= m_scroll->getXScroll();
    topLeft.y -= m_scroll->getYScroll();

    auto bottomRight = ImVec2(topLeft.x + m_cursor->getWidth(), topLeft.y + ImGui::GetFontSize());

    ImGui::GetWindowDrawList()->AddRectFilled(topLeft, bottomRight, m_theme->getCursorColor());
}


// Draws both scrollbars
void TextBox::drawScrollBars() {
    drawHorizontalScrollbar();
    drawVerticalScrollbar();
    auto screenPosition = getTopLeft();
    auto topLeft = ImVec2(screenPosition.x + m_width, screenPosition.y + m_height);
    auto bottomRight = ImVec2(topLeft.x + m_scrollbarSize, topLeft.y + m_scrollbarSize);
    ImGui::GetWindowDrawList()->AddRectFilled(topLeft, bottomRight, m_theme->getScrollbarPrimaryColor());
}

// Draws the entire horizontal scrollbar
void TextBox::drawHorizontalScrollbar() {
    updateHScrollbarRect();
    drawHScrollbarRect();

    auto maxXScroll = m_scroll->getMaxXScroll();

    if (maxXScroll != 0.0f) {
        updateHScrollSelectRect();
        drawRect(m_scroll->getHScrollSelectRect(), m_theme->getScrollbarPrimaryColor());
    }
}

// Draws the entire vertical scrollbar
void TextBox::drawVerticalScrollbar() {
    updateVScrollbarRect();
    drawVScrollbarRect();

    auto maxYScroll = m_scroll->getMaxYScroll();

    if (maxYScroll != 0.0f) {
        updateVScrollSelectRect();
        drawRect(m_scroll->getVScrollSelectRect(), m_theme->getScrollbarPrimaryColor());
    }
}

// Draw the horizontal scrollbar rectangle
void TextBox::drawHScrollbarRect() {
    auto rect = m_scroll->getHScrollbarRect();
    auto topLeft = rect.getTopLeft();
    auto bottomRight = rect.getBottomRight();

    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(topLeft.x - m_scrollbarSize, topLeft.y), ImVec2(bottomRight.x + m_scrollbarSize, bottomRight.y), m_theme->getScrollbarPrimaryColor());
    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(topLeft.x, topLeft.y), ImVec2(bottomRight.x , bottomRight.y), m_theme->getScrollbarSecondaryColor());
}

// Draw the vertical scrollbar rectangle
void TextBox::drawVScrollbarRect() {
    auto rect = m_scroll->getVScrollbarRect();
    auto topLeft = rect.getTopLeft();
    auto bottomRight = rect.getBottomRight();

    ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(topLeft.x, topLeft.y - m_scrollbarSize), ImVec2(bottomRight.x, bottomRight.y + m_scrollbarSize), m_theme->getScrollbarPrimaryColor());
    ImGui::GetWindowDrawList()->AddRectFilled(topLeft, bottomRight, m_theme->getScrollbarSecondaryColor());
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
    auto topLeft = getTopLeft();
    auto textPosition = ImVec2(topLeft.x, topLeft.y + m_height + ImGui::GetFontSize() + 5.0f);
    auto text = getStatusBarText();
    ImGui::GetWindowDrawList()->AddText(textPosition, ImColor(255, 255, 255), text.c_str());
}

std::string TextBox::getStatusBarText() {
    std::stringstream stream;
    stream << (m_file == nullptr ? "New file" : m_file->getName()) << (m_dirty ? "*" : " ") << " |  ";
    stream << "Row: " << m_cursor->getRow() << " Column: " << m_cursor->getCol();
    return stream.str();
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


void TextBox::updateUndoRedo() {
    if (!m_pieceTableInstance->getInstance().isRedoEmpty()) {
        m_pieceTableInstance->getInstance().clearUndoAndRedoStacks();
        m_cursor->clearUndoAndRedoStacks();
    }
}










