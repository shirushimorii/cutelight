#pragma once

#include <functional>

#include "ui/window.h"

namespace CuteLight {

class SettingsView;
struct SettingsData;
class Settings;

class SettingsWindow : public cutefish::ui::Window
{
public:
    SettingsWindow(Settings& settings,
                   std::function<void(const SettingsData&)> onChanged,
                   std::function<void()> onClose);

    void present();
    void hideVisually();

private:
    void applyLayout();

    SettingsView* m_view = nullptr;
};

} // namespace CuteLight
