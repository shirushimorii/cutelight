#include "TrayIcon.h"

#include <memory>

#include "ui/color_mixer.h"
#include "ui/event.h"
#include "ui/icon.h"
#include "ui/view.h"

namespace CuteLight {

namespace {

class TrayView final : public cutefish::ui::View
{
public:
    explicit TrayView(cutefish::ui::StatusItem& item)
        : m_item(item)
    {
        setName("CuteLightTrayIcon");
        m_icon = addChild<cutefish::ui::Icon>("search", 16);
        m_icon->setColor(m_item.foregroundColor());
    }

    bool acceptsPointer() const override { return true; }

    void setForeground(cutefish::ui::Color color)
    {
        m_icon->setColor(color);
    }

    std::function<void()> onActivated;

protected:
    void layout() override
    {
        const auto rect = localRect();
        m_icon->setGeometry({rect.x + (rect.width - 16) / 2,
                             rect.y + (rect.height - 16) / 2,
                             16,
                             16});
    }

    void pointerPress(const cutefish::ui::PointerEvent&) override
    {
        m_pressed = true;
    }

    void pointerRelease(const cutefish::ui::PointerEvent&) override
    {
        const bool activated = m_pressed;
        m_pressed = false;
        if (activated && onActivated)
            onActivated();
    }

private:
    cutefish::ui::StatusItem& m_item;
    cutefish::ui::Icon* m_icon = nullptr;
    bool m_pressed = false;
};

} // namespace

TrayIcon::TrayIcon(Actions actions)
{
    m_item.setIdentifier("com.mori.cutelight.tray");
    m_item.setWidth(28);

    auto view = std::make_unique<TrayView>(m_item);
    auto* trayView = view.get();
    trayView->onActivated = [this] {
        openMenu();
    };
    m_item.setContentView(std::move(view));

    m_item.onAppearanceChanged = [trayView, this] {
        trayView->setForeground(m_item.foregroundColor());
    };
    m_item.onConfigured = [trayView] {
        trayView->requestLayout();
    };

    m_menu.addItem("Show search", actions.show);
    m_menu.addItem("Hide search", actions.hide);
    m_menu.addItem("Clear query", actions.clear);
    m_menu.addSeparator();
    m_menu.addItem("Settings", actions.settings);
    m_menu.addItem("Quit CuteLight", actions.quit);
}

TrayIcon::~TrayIcon() = default;

void TrayIcon::show()
{
    m_item.show();
}

void TrayIcon::openMenu()
{
    m_menu.popupAt(&m_item.window(), {0, m_item.window().size().height});
}

} // namespace CuteLight
