#include "Autostart.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <unistd.h>

namespace CutefishSearch::Autostart {

namespace {

std::string homeDirectory()
{
    const char* home = std::getenv("HOME");
    return home ? home : ".";
}

std::string executablePath()
{
    const std::string installed = homeDirectory() + "/.local/opt/cutefish-search/cutefish-search";
    if (std::filesystem::exists(installed))
        return installed;

    char buffer[4096];
    const ssize_t length = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (length > 0) {
        buffer[length] = '\0';
        return std::string(buffer);
    }
    return "cutefish-search";
}

} // namespace

std::string desktopPath()
{
    const char* config = std::getenv("XDG_CONFIG_HOME");
    std::string root = config && *config ? config : homeDirectory() + "/.config";
    return root + "/autostart/org.cutefish.search.desktop";
}

void setEnabled(bool enabled)
{
    const std::string path = desktopPath();
    std::error_code error;
    std::filesystem::create_directories(std::filesystem::path(path).parent_path(), error);

    // 旧プロトタイプの自動起動エントリだけを取り除く
    std::filesystem::remove(homeDirectory() + "/.config/autostart/com.mori.spotlight.desktop", error);

    if (!enabled) {
        std::filesystem::remove(path, error);
        return;
    }

    std::ofstream file(path, std::ios::trunc);
    if (!file)
        return;
    file << "[Desktop Entry]\n"
         << "Type=Application\n"
         << "Version=1.0\n"
         << "Name=CutefishSearch\n"
         << "Comment=macOS-style Spotlight search for Cutefish\n"
         << "Exec=\"" << executablePath() << "\" --autostart\n"
         << "TryExec=" << executablePath() << "\n"
         << "Icon=org.cutefish.search\n"
         << "Terminal=false\n"
         << "Categories=Utility;\n"
         << "OnlyShowIn=Cutefish;\n"
         << "X-GNOME-Autostart-enabled=true\n";
}

} // namespace CutefishSearch::Autostart
