#pragma once

#include <functional>

#include "settings/Settings.h"
#include "ui/layout.h"

namespace cutefish::ui {
class Button;
class ComboBox;
class Label;
class Slider;
class Switch;
class TextField;
} // namespace cutefish::ui

namespace CutefishSearch {

class ShortcutButton;

// ネイティブウィジェットだけで構成した設定画面
class SettingsView : public cutefish::ui::Column
{
public:
    SettingsView(Settings& settings,
                 std::function<void(const SettingsData&)> onChanged,
                 std::function<void()> onClose);

    void relayout() { layout(); }
    // ウィンドウ座標のクリックをボタンに届けるフォールバック
    bool handleWindowClick(cutefish::ui::PointF windowPoint);
    void saveAndClose();

    bool acceptsPointer() const override { return true; }

protected:
    void pointerPress(const cutefish::ui::PointerEvent& event) override;
    void pointerRelease(const cutefish::ui::PointerEvent& event) override;

private:
    void commit();

    Settings& m_settings;
    SettingsData m_data;
    std::function<void(const SettingsData&)> m_onChanged;
    std::function<void()> m_onClose;
    cutefish::ui::Switch* m_autostart = nullptr;
    cutefish::ui::Switch* m_keepRunning = nullptr;
    cutefish::ui::Switch* m_shortcutEnabled = nullptr;
    ShortcutButton* m_shortcutButton = nullptr;
    cutefish::ui::ComboBox* m_position = nullptr;
    cutefish::ui::ComboBox* m_appearance = nullptr;
    cutefish::ui::Slider* m_tint = nullptr;
    cutefish::ui::Label* m_tintValue = nullptr;
    cutefish::ui::Label* m_shortcutNote = nullptr;
    cutefish::ui::TextField* m_placeholder = nullptr;
    cutefish::ui::Button* m_closeButton = nullptr;
    cutefish::ui::Button* m_saveButton = nullptr;
};

} // namespace CutefishSearch
