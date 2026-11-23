#include "Application.h"

#include <chrono>
#include <string>
#include <string_view>

#include "base/dbus.h"
#include "base/event_loop.h"
#include "base/logging.h"

#include "integration/ControlService.h"
#include "integration/MainMenu.h"
#include "integration/TrayIcon.h"
#include "settings/Settings.h"
#include "window/MainWindow.h"
#include "window/SettingsWindow.h"

namespace CuteLight {

namespace {
const cutefish::base::LogCategory logApplication("cutelight.application");
}

Application::Application(int argc, char** argv)
    : m_native(argc, argv)
{
    for (int i = 1; i < argc; ++i) {
        const std::string_view argument = argv[i];
        if (argument == "--settings")
            m_showSettingsWindow = true;
        else if (argument == "--autostart")
            m_startedHidden = true;
    }
    m_showMainWindow = !m_showSettingsWindow && !m_startedHidden;

    m_native.setApplicationId("com.mori.cutelight");
    CF_INFO(logApplication) << "starting com.mori.cutelight";
}

Application::~Application() = default;

std::optional<int> Application::forwardCommand(int argc, char** argv)
{
    std::string method;
    bool hideOnly = false;
    for (int i = 1; i < argc; ++i) {
        const std::string_view argument = argv[i];
        if (argument == "--toggle")
            method = "Toggle";
        else if (argument == "--show")
            method = "Show";
        else if (argument == "--hide") {
            method = "Hide";
            hideOnly = true;
        } else if (argument == "--settings")
            method = "ShowSettings";
    }
    if (method.empty())
        return std::nullopt;

    cutefish::base::EventLoop loop;
    auto* bus = cutefish::base::dbus::Connection::session(&loop);
    if (!bus)
        return hideOnly ? std::optional<int>(0) : std::nullopt;

    cutefish::base::dbus::Proxy control(bus, "com.mori.CuteLight", "/org/mori/CuteLight",
                                        "org.mori.CuteLight");
    control.setTimeout(std::chrono::milliseconds(500));
    const auto reply = control.call(method);
    if (!reply.isError())
        return 0;

    // 非表示だけのコマンドは、対象がいなければ何もしない
    if (hideOnly)
        return 0;
    return std::nullopt;
}

int Application::run()
{
    m_settings = std::make_unique<Settings>();
    m_window = std::make_unique<MainWindow>(m_settings->data());

    // 外側がクリックされた直後はメニューが開く時間を少しだけ待つ
    m_outsideClickTimer = std::make_unique<cutefish::base::Timer>(&m_native.loop());
    m_outsideClickTimer->setSingleShot(true);
    m_outsideClickTimer->onTimeout = [this] {
        hideAfterOutsideClick();
    };

    m_window->onHidden = [this] {
        m_mainWindowVisible = false;
        setSearchFocused(false);
        if (!m_settings->data().keepRunning)
            quit();
    };
    m_window->onOpenSettings = [this] {
        showSettings();
    };
    m_window->onFocusLost = [this] {
        if (m_outsideClickTimer)
            m_outsideClickTimer->start(std::chrono::milliseconds(150));
    };
    m_window->onSearchFocusChanged = [this](bool focused) {
        if (focused && m_outsideClickTimer)
            m_outsideClickTimer->stop();
        setSearchFocused(focused);
    };

    m_settingsWindow = std::make_unique<SettingsWindow>(
        *m_settings,
        [this](const SettingsData& data) {
            applySettings(data);
        },
        [this] {
            hideSettings();
        });

    const auto show = [this] {
        showWindow();
    };
    const auto hide = [this] {
        hideWindow();
    };
    const auto toggle = [this] {
        toggleWindow();
    };
    const auto clear = [this] {
        if (m_window)
            m_window->clearSearch();
    };
    const auto settings = [this] {
        showSettings();
    };
    const auto quitApp = [this] {
        quit();
    };

    m_control = std::make_unique<ControlService>(m_native.sessionBus(), show, hide, toggle, settings);

    m_mainMenu = std::make_unique<MainMenu>(MainMenu::Actions {
        show, hide, toggle, clear, settings, quitApp,
    });
    m_native.setDockMenu(m_mainMenu->menu());

    m_tray = std::make_unique<TrayIcon>(TrayIcon::Actions {
        show, hide, clear, settings, quitApp,
    });
    m_tray->show();

    if (m_showMainWindow)
        showWindow();
    if (m_showSettingsWindow)
        showSettings();

    return m_native.run();
}

void Application::applySettings(const SettingsData& data)
{
    if (m_window)
        m_window->applySettings(data);
}

void Application::hideAfterOutsideClick()
{
    if (!m_window || !m_mainWindowVisible)
        return;
    // 自分のメニューを操作中に検索バーを消さない
    if ((m_mainMenu && m_mainMenu->isMenuOpen()) || (m_tray && m_tray->isMenuOpen()))
        return;
    if (m_settingsVisible)
        return;
    hideWindow();
}

void Application::toggleWindow()
{
    if (m_mainWindowVisible)
        hideWindow();
    else
        showWindow();
}

void Application::showWindow()
{
    if (!m_window)
        return;
    if (!m_mainWindowVisible) {
        m_mainWindowVisible = true;
        m_window->showCentered();
    } else {
        m_window->activate();
        if (m_window->isVisible())
            m_window->focusSearch();
    }
}

void Application::hideWindow()
{
    if (!m_window || !m_mainWindowVisible)
        return;
    m_window->hideVisually();
    m_mainWindowVisible = false;
    // 非表示時はフォーカスアウトが来ないため、メニューバーを明示的に外す
    setSearchFocused(false);
}

void Application::showSettings()
{
    m_settingsVisible = true;
    if (m_settingsWindow)
        m_settingsWindow->present();
}

void Application::hideSettings()
{
    m_settingsVisible = false;
    if (m_settingsWindow)
        m_settingsWindow->hideVisually();
    if (m_showSettingsWindow && m_settings && !m_settings->data().keepRunning)
        quit();
}

void Application::setSearchFocused(bool focused)
{
    if (!m_mainMenu)
        return;
    m_native.setMainMenu(focused ? m_mainMenu->menu() : nullptr);
}

void Application::quit()
{
    m_native.quit();
}

} // namespace CuteLight
