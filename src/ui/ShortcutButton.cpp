#include "ShortcutButton.h"

#include <utility>

#include "ui/event.h"
#include "ui/keys.h"

#include "settings/Settings.h"

namespace CuteLight {

ShortcutButton::ShortcutButton(std::string text)
    : Button(text)
    , m_idleText(std::move(text))
{
    onClicked = [this] {
        m_capturing = true;
        setText("Press keys...");
        setFocus(cutefish::ui::FocusReason::Other);
    };
}

bool ShortcutButton::keyPress(const cutefish::ui::KeyEvent& event)
{
    if (!m_capturing)
        return Button::keyPress(event);

    if (event.key == cutefish::ui::key::Escape) {
        m_capturing = false;
        setText(m_idleText);
        return true;
    }
    if (event.modifiers == cutefish::ui::NoModifier || event.repeat)
        return true;

    m_capturing = false;
    m_idleText = Settings::shortcutText(event.modifiers, event.key);
    setText(m_idleText);
    if (onCaptured)
        onCaptured(event.modifiers, event.key);
    return true;
}

} // namespace CuteLight
