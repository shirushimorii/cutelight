#pragma once

#include <memory>
#include <optional>

#include "base/timer.h"
#include "ui/application.h"

namespace CutefishSearch {

struct SettingsData;
class ControlService;
class MainMenu;
class MainWindow;
class Settings;
class SettingsWindow;
class TrayIcon;

class Application
{
public:
    Application(int argc, char** argv);
    ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    // 実行中のインスタンスへショートカット用コマンドを渡す
    static std::optional<int> forwardCommand(int argc, char** argv);

    int run();
    void toggleWindow();
    void showWindow();
    void hideWindow();
    void showSettings();
    void hideSettings();

private:
    void applySettings(const SettingsData& data);
    void setSearchFocused(bool focused);
    void scheduleSearchFocus();
    void hideAfterOutsideClick();
    void quit();

    cutefish::ui::Application m_native;
    std::unique_ptr<Settings> m_settings;
    std::unique_ptr<MainWindow> m_window;
    std::unique_ptr<SettingsWindow> m_settingsWindow;
    std::unique_ptr<ControlService> m_control;
    std::unique_ptr<MainMenu> m_mainMenu;
    std::unique_ptr<TrayIcon> m_tray;
    std::unique_ptr<cutefish::base::Timer> m_outsideClickTimer;
    std::unique_ptr<cutefish::base::Timer> m_focusTimer;
    int m_focusRetries = 0;
    bool m_showMainWindow = true;
    bool m_showSettingsWindow = false;
    bool m_startedHidden = false;
    bool m_mainWindowVisible = false;
    bool m_settingsVisible = false;
};

} // namespace CutefishSearch
