#include "ShortcutButton.h"

#include <utility>

#include "ui/event.h"
#include "ui/keys.h"

#include "settings/Settings.h"

namespace CuteLight {

namespace {

// XKB の修飾キー keysym。単押しではショートカットを確定せず、
// 後続のキー（または離す操作）を待つために使う。
bool isModifierKeyImpl(uint32_t key)
{
    switch (key) {
    case 0xffe1: // Shift_L
    case 0xffe2: // Shift_R
    case 0xffe3: // Control_L
    case 0xffe4: // Control_R
    case 0xffe7: // Meta_L
    case 0xffe8: // Meta_R
    case 0xffe9: // Alt_L
    case 0xffea: // Alt_R
    case 0xffeb: // Super_L
    case 0xffec: // Super_R
    case 0xffed: // Hyper_L
    case 0xffee: // Hyper_R
    case 0xffe5: // Caps_Lock
    case 0xffb7: // Num_Lock
    case 0xff14: // Scroll_Lock
    case 0xff7e: // Mode_switch
    case 0xfe03: // ISO_Level3_Shift
        return true;
    default:
        return false;
    }
}

} // namespace

ShortcutButton::ShortcutButton(std::string text)
    : Button(text)
    , m_idleText(std::move(text))
{
    onClicked = [this] {
        m_capturing = true;
        m_pendingModifiers = 0;
        setText("Press keys...");
        setFocus(cutefish::ui::FocusReason::Other);
    };
}

bool ShortcutButton::isModifierKey(uint32_t key)
{
    return isModifierKeyImpl(key);
}

void ShortcutButton::finish(uint32_t modifiers, uint32_t key)
{
    m_capturing = false;
    m_pendingModifiers = 0;
    m_idleText = Settings::shortcutText(modifiers, key);
    setText(m_idleText);
    if (onCaptured)
        onCaptured(modifiers, key);
}

void ShortcutButton::cancel()
{
    m_capturing = false;
    m_pendingModifiers = 0;
    setText(m_idleText);
}

bool ShortcutButton::keyPress(const cutefish::ui::KeyEvent& event)
{
    if (!m_capturing)
        return Button::keyPress(event);

    if (event.repeat)
        return true;
    if (event.key == cutefish::ui::key::Escape) {
        cancel();
        return true;
    }

    // 修飾キー単体では確定しない。 Ctrl+Space のような組合せを
    // 正しく取り込むため、押された修飾キーを積んでおく。
    if (isModifierKey(event.key)) {
        m_pendingModifiers |= event.modifiers;
        return true;
    }

    const uint32_t modifiers = m_pendingModifiers | event.modifiers;
    if (modifiers == cutefish::ui::NoModifier)
        return true;

    finish(modifiers, event.key);
    return true;
}

bool ShortcutButton::keyRelease(const cutefish::ui::KeyEvent& event)
{
    if (!m_capturing)
        return Button::keyRelease(event);

    // 修飾キーを押して離すだけで終わった場合は、その修飾キー単体の
    // ショートカットとして確定する（例: Super のみ）。
    if (isModifierKey(event.key) && m_pendingModifiers != cutefish::ui::NoModifier)
        finish(m_pendingModifiers, 0);
    return true;
}

} // namespace CuteLight
