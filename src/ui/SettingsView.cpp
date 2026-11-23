#include "SettingsView.h"

#include <string>
#include <utility>

#include "base/logging.h"

#include "ui/button.h"
#include "ui/combo_box.h"
#include "ui/label.h"
#include "ui/slider.h"
#include "ui/switch.h"
#include "ui/text_field.h"
#include "ui/window_header.h"

#include "ShortcutButton.h"
#include "settings/Autostart.h"

namespace CuteLight {

namespace {
const cutefish::base::LogCategory logSettings("cutelight.settings");

cutefish::ui::Label* addHeading(cutefish::ui::Column* column, const std::string& text)
{
    auto* label = column->addChild<cutefish::ui::Label>(text);
    label->setFontSize(15);
    label->setWeight(600);
    cutefish::ui::LayoutHints hints;
    hints.fillWidth = true;
    hints.preferredHeight = 24;
    label->setLayoutHints(hints);
    return label;
}

cutefish::ui::Label* addNote(cutefish::ui::Column* column, const std::string& text)
{
    auto* label = column->addChild<cutefish::ui::Label>(text);
    label->setFontSize(12);
    label->setWrap(true);
    cutefish::ui::LayoutHints hints;
    hints.fillWidth = true;
    label->setLayoutHints(hints);
    return label;
}

cutefish::ui::Row* addField(cutefish::ui::Column* column,
                            const std::string& labelText,
                            cutefish::ui::View* control)
{
    auto* row = column->addChild<cutefish::ui::Row>();
    row->setSpacing(12);
    cutefish::ui::LayoutHints rowHints;
    rowHints.fillWidth = true;
    rowHints.preferredHeight = 30;
    row->setLayoutHints(rowHints);

    auto* label = row->addChild<cutefish::ui::Label>(labelText);
    label->setVerticallyCentered(true);
    cutefish::ui::LayoutHints labelHints;
    labelHints.fillWidth = true;
    label->setLayoutHints(labelHints);

    cutefish::ui::LayoutHints controlHints;
    controlHints.alignment = cutefish::ui::Alignment::Center;
    controlHints.preferredWidth = 240;
    control->setLayoutHints(controlHints);
    row->addChild(std::unique_ptr<cutefish::ui::View>(control));
    return row;
}

} // namespace

SettingsView::SettingsView(Settings& settings,
                           std::function<void(const SettingsData&)> onChanged,
                           std::function<void()> onClose)
    : m_settings(settings)
    , m_data(settings.data())
    , m_onChanged(std::move(onChanged))
    , m_onClose(std::move(onClose))
{
    setName("SettingsView");
    setSpacing(14);
    setMargins({24, 12, 24, 20});

    // 上部のバーを掴んで設定ウィンドウを動かせるようにする
    auto* header = addChild<cutefish::ui::WindowHeader>(40);
    header->setName("SettingsDragHandle");
    header->setToolTip("Drag this bar to move the settings window.");
    header->setCloseVisible(false);
    header->setMinimizeVisible(false);
    header->setMaximizeVisible(false);
    header->setMaximizeEnabled(false);
    auto* title = header->setContent<cutefish::ui::Label>("CuteLight settings");
    title->setFontSize(16);
    title->setWeight(600);
    cutefish::ui::LayoutHints titleHints;
    titleHints.fillWidth = true;
    titleHints.preferredHeight = 32;
    title->setLayoutHints(titleHints);
    cutefish::ui::LayoutHints headerHints;
    headerHints.fillWidth = true;
    headerHints.preferredHeight = 40;
    header->setLayoutHints(headerHints);

    addHeading(this, "Getting started");
    m_autostart = addChild<cutefish::ui::Switch>("Open CuteLight when I sign in");
    m_autostart->setChecked(m_data.autostart);
    m_autostart->onToggled = [this](bool checked) {
        m_data.autostart = checked;
    };

    m_keepRunning = addChild<cutefish::ui::Switch>("Keep CuteLight running in the background");
    m_keepRunning->setChecked(m_data.keepRunning);
    m_keepRunning->onToggled = [this](bool checked) {
        m_data.keepRunning = checked;
    };

    m_shortcutEnabled = addChild<cutefish::ui::Switch>("Open with a shortcut");
    m_shortcutEnabled->setChecked(m_data.launchWithShortcut);
    m_shortcutEnabled->onToggled = [this](bool checked) {
        m_data.launchWithShortcut = checked;
    };

    m_shortcutButton = new ShortcutButton(Settings::shortcutText(m_data.shortcutModifiers,
                                                                  m_data.shortcutKey));
    addField(this, "Shortcut", m_shortcutButton);
    m_shortcutButton->onCaptured = [this](uint32_t modifiers, uint32_t key) {
        m_data.shortcutModifiers = modifiers;
        m_data.shortcutKey = key;
    };
    m_shortcutNote = addNote(this,
                             "Turn this on to use the shortcut. CuteLight runs a small helper in "
                             "the background so the shortcut can show or hide CuteLight even when "
                             "you are in another app.");

    addHeading(this, "Search window");
    m_position = new cutefish::ui::ComboBox({"Top", "Top middle", "Centre", "Bottom middle", "Bottom"});
    m_position->setCurrentIndex(int(m_data.position));
    m_position->onActivated = [this](int index) {
        m_data.position = static_cast<SearchPosition>(index);
    };
    addField(this, "Where it appears", m_position);

    m_placeholder = new cutefish::ui::TextField(m_data.placeholder);
    m_placeholder->setName("PlaceholderField");
    m_placeholder->setFontSize(13);
    m_placeholder->setTextMargins(8, 0);
    m_placeholder->onTextChanged = [this] {
        m_data.placeholder = m_placeholder->text();
    };
    addField(this, "Text when the box is empty", m_placeholder);
    addNote(this, "This is the friendly text you see before you type anything.");

    addHeading(this, "Look and feel");
    m_appearance = new cutefish::ui::ComboBox({"Tinted", "Glassy"});
    m_appearance->setCurrentIndex(int(m_data.appearance));
    m_appearance->onActivated = [this](int index) {
        m_data.appearance = static_cast<AppearanceMode>(index);
    };
    addField(this, "Glass style", m_appearance);

    auto* tintRow = addChild<cutefish::ui::Row>();
    tintRow->setSpacing(12);
    cutefish::ui::LayoutHints tintRowHints;
    tintRowHints.fillWidth = true;
    tintRowHints.preferredHeight = 30;
    tintRow->setLayoutHints(tintRowHints);
    auto* tintLabel = tintRow->addChild<cutefish::ui::Label>("Tint strength");
    tintLabel->setVerticallyCentered(true);
    cutefish::ui::LayoutHints tintLabelHints;
    tintLabelHints.fillWidth = true;
    tintLabel->setLayoutHints(tintLabelHints);
    m_tintValue = tintRow->addChild<cutefish::ui::Label>(std::to_string(m_data.tint) + "%");
    m_tintValue->setVerticallyCentered(true);
    cutefish::ui::LayoutHints valueHints;
    valueHints.preferredWidth = 44;
    valueHints.horizontalAlignment = cutefish::ui::Alignment::End;
    m_tintValue->setLayoutHints(valueHints);

    m_tint = addChild<cutefish::ui::Slider>();
    m_tint->setRange(0, 100);
    m_tint->setStepSize(5);
    m_tint->setValue(m_data.tint);
    m_tint->setValueToolTip([](double value) { return std::to_string(int(value)) + "%"; });
    m_tint->onMoved = [this](double value) {
        m_data.tint = int(value);
        m_tintValue->setText(std::to_string(m_data.tint) + "%");
    };
    cutefish::ui::LayoutHints tintHints;
    tintHints.fillWidth = true;
    tintHints.preferredHeight = 30;
    m_tint->setLayoutHints(tintHints);

    addStretch();

    auto* buttons = addChild<cutefish::ui::Row>();
    buttons->setSpacing(10);
    cutefish::ui::LayoutHints buttonHints;
    buttonHints.alignment = cutefish::ui::Alignment::End;
    buttonHints.preferredHeight = 32;
    buttons->setLayoutHints(buttonHints);

    m_closeButton = buttons->addChild<cutefish::ui::Button>("Close");
    m_closeButton->setName("CloseButton");
    cutefish::ui::LayoutHints closeHints;
    closeHints.preferredWidth = 90;
    m_closeButton->setLayoutHints(closeHints);
    m_closeButton->onClicked = [this] {
        CF_INFO(logSettings) << "Close button clicked";
        // 保存せずに閉じる
        if (m_onClose)
            m_onClose();
    };

    m_saveButton = buttons->addChild<cutefish::ui::Button>("Save and Close");
    m_saveButton->setName("SaveCloseButton");
    cutefish::ui::LayoutHints saveHints;
    saveHints.preferredWidth = 130;
    m_saveButton->setLayoutHints(saveHints);
    m_saveButton->onClicked = [this] {
        CF_INFO(logSettings) << "Save and Close button clicked";
        saveAndClose();
    };
}

void SettingsView::commit()
{
    // 保存時にだけ自動起動エントリと実行中設定へ反映する
    Autostart::setEnabled(m_data.autostart);
    m_settings.save(m_data);
    if (m_onChanged)
        m_onChanged(m_data);
}

void SettingsView::pointerPress(const cutefish::ui::PointerEvent& event)
{
    CF_INFO(logSettings) << "pointer press in SettingsView";
    Column::pointerPress(event);
}

bool SettingsView::handleWindowClick(cutefish::ui::PointF windowPoint)
{
    auto inside = [](cutefish::ui::View* view, cutefish::ui::PointF p) {
        if (!view)
            return false;
        const auto topLeft = view->mapToWindow({0, 0});
        const cutefish::ui::RectF rect {topLeft.x, topLeft.y, view->geometry().width,
                                        view->geometry().height};
        return rect.contains(p);
    };
    if (m_closeButton && m_closeButton->isEnabled() && m_closeButton->isVisible() && inside(m_closeButton, windowPoint)) {
        CF_INFO(logSettings) << "Close button hit by geometry";
        if (m_onClose)
            m_onClose();
        return true;
    }
    if (m_saveButton && m_saveButton->isEnabled() && m_saveButton->isVisible() && inside(m_saveButton, windowPoint)) {
        CF_INFO(logSettings) << "Save and Close button hit by geometry";
        saveAndClose();
        return true;
    }
    return false;
}

void SettingsView::pointerRelease(const cutefish::ui::PointerEvent& event)
{
    CF_INFO(logSettings) << "pointer release in SettingsView";
    // Wayland のテスト合成入力で子ボタンの onClicked が届かない場合に備え、
    // ボタンの矩形内リリースでも確実に実行する
    if (handleWindowClick(event.position))
        return;
    Column::pointerRelease(event);
}

void SettingsView::saveAndClose()
{
    commit();
    if (m_onClose)
        m_onClose();
}

} // namespace CuteLight
