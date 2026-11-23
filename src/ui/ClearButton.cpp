#include "ClearButton.h"

#include "ui/color_id.h"
#include "ui/color_mixer.h"
#include "ui/painter.h"

#include "DesignTokens.h"

namespace CutefishSearch {

ClearButton::ClearButton()
{
    setName("ClearButton");
}

void ClearButton::setColor(cutefish::ui::ColorRecipe color)
{
    m_color = color;
    requestPaint();
}

cutefish::ui::SizeF ClearButton::sizeHint() const
{
    return {Tokens::Search::ClearButtonSize, Tokens::Search::ClearButtonSize};
}

void ClearButton::paint(cutefish::ui::Painter& painter)
{
    const auto color = m_color
        ? m_color->resolve()
        : cutefish::ui::ColorRecipe(cutefish::ui::ColorId::DisabledText).resolve();
    const double inset = 7;
    const auto rect = localRect();
    const double left = rect.x + inset;
    const double top = rect.y + inset;
    const double right = rect.right() - inset;
    const double bottom = rect.bottom() - inset;

    painter.drawLine({left, top}, {right, bottom}, color, 1.5);
    painter.drawLine({right, top}, {left, bottom}, color, 1.5);
}

void ClearButton::pointerPress(const cutefish::ui::PointerEvent&)
{
    m_pressed = true;
}

void ClearButton::pointerRelease(const cutefish::ui::PointerEvent&)
{
    const bool clicked = m_pressed;
    m_pressed = false;
    if (clicked && onClicked)
        onClicked();
}

} // namespace CutefishSearch
