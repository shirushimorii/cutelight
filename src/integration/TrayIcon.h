#pragma once

#include <functional>

#include "ui/menu.h"
#include "ui/status_item.h"

namespace CuteLight {

// メニューバーに常駐するアプリケーションアイコン
class TrayIcon
{
public:
    struct Actions {
        std::function<void()> show;
        std::function<void()> hide;
        std::function<void()> clear;
        std::function<void()> settings;
        std::function<void()> quit;
    };

    explicit TrayIcon(Actions actions);
    ~TrayIcon();

    void show();
    bool isMenuOpen() const { return m_menu.isOpen(); }

private:
    void openMenu();

    cutefish::ui::StatusItem m_item;
    cutefish::ui::Menu m_menu;
};

} // namespace CuteLight
