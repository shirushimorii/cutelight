#pragma once

#include <cstdint>
#include <string>

namespace CutefishSearch {

enum class SearchPosition {
    Top,
    TopMid,
    Centre,
    BottomMid,
    Bottom,
};

enum class AppearanceMode {
    Tinted,
    Glassy,
};

struct SettingsData {
    bool autostart = true;
    bool keepRunning = true;
    bool launchWithShortcut = true;
    uint32_t shortcutModifiers = 8; // Super
    uint32_t shortcutKey = 0x0020;   // Space
    SearchPosition position = SearchPosition::TopMid;
    AppearanceMode appearance = AppearanceMode::Glassy;
    int tint = 55;
    std::string placeholder = "Cutefish Search Search";
};

class Settings
{
public:
    explicit Settings(std::string path = {});

    const SettingsData& data() const { return m_data; }

    void save(SettingsData data);

    static std::string defaultPath();
    static double positionRatio(SearchPosition position);
    static std::string shortcutText(uint32_t modifiers, uint32_t key);
    static const char* positionName(SearchPosition position);
    static const char* appearanceName(AppearanceMode mode);

private:
    std::string m_path;
    SettingsData m_data;
};

} // namespace CutefishSearch
