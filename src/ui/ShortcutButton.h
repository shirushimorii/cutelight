#pragma once

#include <functional>
#include <string>

#include "ui/button.h"

namespace cutefish::ui {
struct KeyEvent;
}

namespace CuteLight {

// 押下後にショートカットのキー入力を待ち受けるボタン
class ShortcutButton : public cutefish::ui::Button
{
public:
    explicit ShortcutButton(std::string text = {});

    bool keyPress(const cutefish::ui::KeyEvent& event) override;

    std::function<void(uint32_t modifiers, uint32_t key)> onCaptured;

private:
    std::string m_idleText;
    bool m_capturing = false;
};

} // namespace CuteLight
