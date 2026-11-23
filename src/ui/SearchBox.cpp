#include "SearchBox.h"

#include <utility>

#include "ui/color_id.h"
#include "ui/color_mixer.h"
#include "ui/event.h"
#include "ui/icon.h"
#include "ui/keys.h"
#include "ui/layout.h"
#include "ui/text_field.h"
#include "ui/theme.h"

#include "ClearButton.h"
#include "DesignTokens.h"

namespace CuteLight {

namespace {

class QueryField final : public cutefish::ui::TextField
{
public:
    std::function<bool(uint32_t key)> onSpecialKey;
    std::function<void(bool)> onFocusChanged;

protected:
    bool keyPress(const cutefish::ui::KeyEvent& event) override
    {
        if (onSpecialKey && onSpecialKey(event.key))
            return true;
        return TextField::keyPress(event);
    }

    void focusIn(cutefish::ui::FocusReason reason) override
    {
        TextField::focusIn(reason);
        if (onFocusChanged)
            onFocusChanged(true);
    }

    void focusOut() override
    {
        TextField::focusOut();
        if (onFocusChanged)
            onFocusChanged(false);
    }
};

} // namespace

SearchBox::SearchBox()
{
    setName("SearchBox");
    setClipsChildren(true);

    setSpacing(Tokens::Search::IconTextSpacing);
    setMargins({Tokens::Search::HorizontalPadding, 0, Tokens::Search::HorizontalPadding, 0});

    m_icon = addChild<cutefish::ui::Icon>("search", Tokens::Search::IconSize);
    cutefish::ui::LayoutHints iconHints;
    iconHints.alignment = cutefish::ui::Alignment::Center;
    m_icon->setLayoutHints(iconHints);

    auto* field = addChild<QueryField>();
    field->setName("QueryField");
    field->onSpecialKey = [this](uint32_t key) {
        return onSpecialKey ? onSpecialKey(key) : false;
    };
    field->onFocusChanged = [this](bool focused) {
        if (onFocusChanged)
            onFocusChanged(focused);
    };
    m_text = field;
    m_text->setPlaceholder("Cutelight Search");
    m_text->setFontSize(Tokens::Search::FontSize);
    m_text->setFontWeight(Tokens::Search::FontWeight);
    m_text->setFrameVisible(false);
    m_text->setTextMargins(0, 0);
    m_text->setFadesWhenDisabled(false);
    cutefish::ui::LayoutHints textHints;
    textHints.fillWidth = true;
    textHints.fillHeight = true;
    m_text->setLayoutHints(textHints);

    m_clear = addChild<ClearButton>();
    cutefish::ui::LayoutHints clearHints;
    clearHints.alignment = cutefish::ui::Alignment::Center;
    m_clear->setLayoutHints(clearHints);
    m_clear->setVisible(false);
    m_clear->onClicked = [this] {
        clear();
        focus();
    };

    m_text->onTextChanged = [this] {
        updateClearButton();
        if (onTextChanged)
            onTextChanged();
    };

    updateTheme();
}

const std::string& SearchBox::text() const
{
    return m_text->text();
}

void SearchBox::setPlaceholder(std::string text)
{
    m_text->setPlaceholder(std::move(text));
}

void SearchBox::clear()
{
    m_text->clear();
}

void SearchBox::focus()
{
    m_text->setFocus(cutefish::ui::FocusReason::Other);
}

void SearchBox::updateTheme()
{
    const auto& theme = cutefish::ui::Theme::current();
    const bool dark = theme.darkMode;
    const auto primary = cutefish::ui::ColorRecipe(Tokens::Palette::primaryText(dark));
    const auto secondary = cutefish::ui::ColorRecipe(Tokens::Palette::secondaryText(dark));

    m_text->setFontFamily(theme.fontFamily);
    m_text->setTextColor(primary);
    m_text->setPlaceholderColor(secondary);
    m_icon->setColor(secondary);
    m_clear->setColor(secondary);

    requestPaint();
}

cutefish::ui::SizeF SearchBox::sizeHint() const
{
    return {Tokens::Window::PreferredWidth, Tokens::Search::Height};
}

void SearchBox::updateClearButton()
{
    m_clear->setVisible(!m_text->text().empty());
}

} // namespace CuteLight
