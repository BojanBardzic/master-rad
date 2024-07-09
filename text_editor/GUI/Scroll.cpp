//
// Created by bbard on 7/8/2024.
//

#include "Scroll.h"

Scroll::Scroll(LineBuffer *lineBuffer, Cursor *cursor, Font *font)
    : m_lineBuffer(lineBuffer), m_cursor(cursor), m_font(font), m_xScroll(0.0f), m_yScroll(0.0f),
      m_maxXScroll(0.0f), m_maxYScroll(0.0f) {}

Scroll::~Scroll() {}

void Scroll::updateScroll(float &width, float &height) {
    updateXScroll(width);
    updateYScroll(height);
}

void Scroll::updateXScroll(float &width) {
    ImGui::PushFont(m_font->getFont());

    auto cursorRow = m_cursor->getRow();
    auto cursorCol = m_cursor->getCol();

    auto advance = m_cursor->getXAdvance();

    if (advance - m_xScroll > width) {
        m_xScroll = advance - width;
    } else if (advance != 0 && advance < m_xScroll) {
        m_xScroll = advance - ImGui::GetFont()->GetCharAdvance(m_lineBuffer->lineAt(cursorRow-1)[cursorCol-1]);
    } else if (advance == 0) {
        m_xScroll = 0;
    }

    ImGui::PopFont();
}

void Scroll::updateYScroll(float& height) {
    ImGui::PushFont(m_font->getFont());

    auto cursorRow = m_cursor->getRow();
    auto lineHeight = ImGui::GetFontSize();

    std::cerr << "ROW: " << cursorRow << std::endl;

    if (cursorRow * lineHeight - m_yScroll > height) {
        m_yScroll = cursorRow * lineHeight - height;
    } else if (cursorRow != 1 && m_yScroll >= (cursorRow * lineHeight)) {
        m_yScroll = (cursorRow-1) * lineHeight;
    } else if (cursorRow == 1) {
        m_yScroll = 0;
    }

    ImGui::PopFont();
}

void Scroll::updateMaxScroll(float& width, float& height) {
    updateMaxXScroll(width);
    updateMaxYScroll(height);
}

void Scroll::updateMaxXScroll(float& width) {
    ImGui::PushFont(m_font->getFont());
    std::cerr << "Entered MaxXScroll" << std::endl;
    float maxAdvance = 0.0f;

    for (size_t i=0; i<m_lineBuffer->getSize(); ++i) {
        auto advance = m_cursor->getXAdvance(m_lineBuffer->lineAt(i));
        if (advance > maxAdvance)
            maxAdvance = advance;
    }

    m_maxXScroll = std::max(0.0f, maxAdvance-width);
    std::cerr << "Exited MaxXScroll" << std::endl;
    ImGui::PopFont();
}

void Scroll::updateMaxYScroll(float& height) {
    ImGui::PushFont(m_font->getFont());
    m_maxYScroll = std::max(0.0f, (m_lineBuffer->getSize() * ImGui::GetFontSize()) - height);
    ImGui::PopFont();
}

void Scroll::mouseWheelXScroll(float& mouseWheel) {
    ImGui::PushFont(m_font->getFont());
    auto lineHeight = ImGui::GetFontSize();

    if (mouseWheel < 0.0f) {
        m_xScroll = std::min(m_xScroll + lineHeight, m_maxXScroll);
    } else {
        m_xScroll = std::max(m_xScroll - lineHeight, 0.0f);
    }

    ImGui::PopFont();
}

void Scroll::mouseWheelYScroll(float& mouseWheel) {
    ImGui::PushFont(m_font->getFont());
    auto lineHeight = ImGui::GetFontSize();

    int numRowsScrolled = (int) (m_yScroll / lineHeight);
    m_yScroll = numRowsScrolled * lineHeight;

    if (mouseWheel < 0.0f) {
        std::cerr << "Max scroll: " << m_maxYScroll << std::endl;
        std::cerr << "Scrolling down" << std::endl;
        m_yScroll = std::min(m_yScroll + lineHeight, m_maxYScroll);
    } else {
        m_yScroll = std::max(m_yScroll - lineHeight, 0.0f);
    }

    ImGui::PopFont();
}

void Scroll::init(float& width, float& height) {
    if (m_init)
        return;

    m_init = true;
    updateMaxScroll(width, height);
}

bool Scroll::isInit() const { return m_init; }

float Scroll::getXScroll() const { return m_xScroll; }

float Scroll::getYScroll() const { return m_yScroll; }

float Scroll::getMaxXScroll() const { return m_maxXScroll; }

float Scroll::getMaxYScroll() const { return m_maxYScroll; }





