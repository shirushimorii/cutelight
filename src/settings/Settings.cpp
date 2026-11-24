#include "Settings.h"

#include <cstdlib>
#include <filesystem>
#include <sstream>

#include "base/settings.h"
#include "ui/event.h"
#include "ui/keys.h"

namespace CuteLight {

namespace {

SearchPosition readPosition(const std::string& value)
{
    if (value == "top")
        return SearchPosition::Top;
    if (value == "top-mid")
        return SearchPosition::TopMid;
    if (value == "centre")
        return SearchPosition::Centre;
    if (value == "bottom-mid")
        return SearchPosition::BottomMid;
    if (value == "bottom")
        return SearchPosition::Bottom;
    return SearchPosition::TopMid;
}

AppearanceMode readAppearance(const std::string& value)
{
    return value == "tinted" ? AppearanceMode::Tinted : AppearanceMode::Glassy;
}

std::string keyName(uint32_t key)
{
    using namespace cutefish::ui;

    switch (key) {
    case key::Space:
        return "Space";
    case key::BackSpace:
        return "Backspace";
    case key::Tab:
        return "Tab";
    case key::Return:
        return "Return";
    case key::Escape:
        return "Escape";
    case key::Home:
        return "Home";
    case key::Left:
        return "Left";
    case key::Up:
        return "Up";
    case key::Right:
        return "Right";
    case key::Down:
        return "Down";
    case key::PageUp:
        return "PageUp";
    case key::PageDown:
        return "PageDown";
    case key::End:
        return "End";
    case key::Insert:
        return "Insert";
    case key::Menu:
        return "Menu";
    case key::Delete:
        return "Delete";
    default:
        break;
    }
    if (key >= key::F1 && key <= key::F4 + 8)
        return "F" + std::to_string(key - key::F1 + 1);
    if (key >= 'a' && key <= 'z')
        return std::string(1, char(key - 'a' + 'A'));
    if (key >= '0' && key <= '9')
        return std::string(1, char(key));
    std::ostringstream out;
    out << "0x" << std::hex << key;
    return out.str();
}

} // namespace

Settings::Settings(std::string path)
    : m_path(std::move(path))
{
    if (m_path.empty())
        m_path = defaultPath();

    cutefish::base::Settings file(m_path);
    m_data.autostart = file.boolValue("General/autostart", true);
    m_data.keepRunning = file.boolValue("General/keepRunning", true);
    m_data.launchWithShortcut = file.boolValue("General/launchWithShortcut", true);
    m_data.shortcutModifiers =
        uint32_t(file.intValue("General/shortcutModifiers", m_data.shortcutModifiers));
    m_data.shortcutKey = uint32_t(file.intValue("General/shortcutKey", m_data.shortcutKey));
    m_data.position = readPosition(file.stringValue("General/position", "top-mid"));
    m_data.appearance = readAppearance(file.stringValue("General/appearance", "glassy"));
    m_data.tint = int(file.intValue("General/tint", m_data.tint));
    m_data.placeholder = file.stringValue("General/placeholder", m_data.placeholder);
}

void Settings::save(SettingsData data)
{
    m_data = data;
    std::error_code error;
    std::filesystem::create_directories(std::filesystem::path(m_path).parent_path(), error);
    if (error)
        return;

    cutefish::base::Settings file(m_path);
    file.setValue("General/autostart", m_data.autostart);
    file.setValue("General/keepRunning", m_data.keepRunning);
    file.setValue("General/launchWithShortcut", m_data.launchWithShortcut);
    file.setValue("General/shortcutModifiers", int64_t(m_data.shortcutModifiers));
    file.setValue("General/shortcutKey", int64_t(m_data.shortcutKey));
    file.setValue("General/position", positionName(m_data.position));
    file.setValue("General/appearance", appearanceName(m_data.appearance));
    file.setValue("General/tint", int64_t(m_data.tint));
    file.setValue("General/placeholder", m_data.placeholder);
    file.sync();
}

std::string Settings::defaultPath()
{
    const char* xdg = std::getenv("XDG_CONFIG_HOME");
    const char* home = std::getenv("HOME");
    std::string root = xdg && *xdg ? xdg : (home ? std::string(home) + "/.config" : std::string("."));
    return root + "/cutelight/cutelight.conf";
}

double Settings::positionRatio(SearchPosition position)
{
    switch (position) {
    case SearchPosition::Top:
        return 0.10;
    case SearchPosition::TopMid:
        return 0.37;
    case SearchPosition::Centre:
        return 0.50;
    case SearchPosition::BottomMid:
        return 0.68;
    case SearchPosition::Bottom:
        return 0.84;
    }
    return 0.37;
}

std::string Settings::shortcutText(uint32_t modifiers, uint32_t key)
{
    using namespace cutefish::ui;

    // key == 0 は修飾キーのみのショートカット（例: Super のみ）を表す。
    std::string text;
    if (modifiers & SuperModifier)
        text += "Super+";
    if (modifiers & ControlModifier)
        text += "Ctrl+";
    if (modifiers & AltModifier)
        text += "Alt+";
    if (modifiers & ShiftModifier)
        text += "Shift+";

    if (key == 0) {
        if (!text.empty())
            text.pop_back(); // 末尾の '+' を取り除く
        return text.empty() ? std::string("None") : text;
    }
    text += keyName(key);
    return text;
}

const char* Settings::positionName(SearchPosition position)
{
    switch (position) {
    case SearchPosition::Top:
        return "top";
    case SearchPosition::TopMid:
        return "top-mid";
    case SearchPosition::Centre:
        return "centre";
    case SearchPosition::BottomMid:
        return "bottom-mid";
    case SearchPosition::Bottom:
        return "bottom";
    }
    return "top-mid";
}

const char* Settings::appearanceName(AppearanceMode mode)
{
    return mode == AppearanceMode::Tinted ? "tinted" : "glassy";
}

} // namespace CuteLight
