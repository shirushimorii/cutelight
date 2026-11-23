#pragma once

#include <functional>
#include <memory>

#include "ui/menu.h"

namespace CuteLight {

// メニューバーに出すアプリケーションメニュー
class MainMenu
{
public:
    struct Actions {
        std::function<void()> show;
        std::function<void()> hide;
        std::function<void()> toggle;
        std::function<void()> clear;
        std::function<void()> settings;
        std::function<void()> quit;
    };

    explicit MainMenu(Actions actions);
    ~MainMenu();

    cutefish::ui::Menu* menu() const { return m_menu.get(); }
    bool isMenuOpen() const { return m_menu && m_menu->isOpen(); }

private:
    std::unique_ptr<cutefish::ui::Menu> m_menu;
};

} // namespace CuteLight
