#include "SettingsWindow.h"

#include <memory>
#include <utility>

#include "ui/color_id.h"
#include "ui/color_mixer.h"
#include "ui/event.h"
#include "ui/event.h"
#include "ui/keys.h"
#include "ui/theme.h"

#include "ui/SettingsView.h"

namespace CuteLight {

namespace {
constexpr double kWidth = 540.0;
constexpr double kHeight = 720.0;
} // namespace

SettingsWindow::SettingsWindow(Settings& settings,
                               std::function<void(const SettingsData&)> onChanged,
                               std::function<void()> onClose)
{
    setTitle("CuteLight Settings");
    setSkipTaskbar(false);
    setUserResizable(false);
    setLevel(cutefish::ui::WindowLevel::Floating);
    setCastsShadow(true);
    setResizeAroundCenter(false);
    setMinimumSize({kWidth, kHeight});
    setMaximumSize({kWidth, kHeight});
    resize({kWidth, kHeight});
    setBackgroundColor(cutefish::ui::ColorRecipe(cutefish::ui::ColorId::Background));

    auto view = std::make_unique<SettingsView>(settings, std::move(onChanged), onClose);
    m_view = view.get();
    setContentView(std::move(view));
    applyLayout();

    onCloseRequested = [this, onClose] {
        if (onClose)
            onClose();
    };
    addShortcut(cutefish::ui::NoModifier, cutefish::ui::key::Escape, [this, onClose] {
        if (onClose)
            onClose();
    });
    addShortcut(cutefish::ui::ControlModifier, 's', [this] {
        if (m_view)
            m_view->saveAndClose();
    });
    // 合成入力やフォーカス状態によってボタンへ届かないクリックの保険
    onPointerPressed = [this](cutefish::ui::PointF position, cutefish::ui::MouseButton button) {
        if (button != cutefish::ui::MouseButton::Left || !m_view)
            return;
        m_view->handleWindowClick(position);
    };
}

void SettingsWindow::present()
{
    setOpacity(1);
    setInputTransparent(false);
    show();
    activate();
    applyLayout();
}

void SettingsWindow::hideVisually()
{
    // 設定ウィンドウも Wayland では hide() だけだと surface が残るため隠す
    setOpacity(0);
    setInputTransparent(true);
    hide();
}



void SettingsWindow::applyLayout()
{
    if (!m_view)
        return;
    const cutefish::ui::RectF rect {0, 0, kWidth, kHeight};
    m_view->setGeometry(rect);
    m_view->relayout();
}

} // namespace CuteLight
