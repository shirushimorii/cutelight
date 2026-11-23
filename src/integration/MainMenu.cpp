#include "MainMenu.h"

#include "ui/menu.h"

namespace CutefishSearch {

MainMenu::MainMenu(Actions actions)
{
    m_menu = std::make_unique<cutefish::ui::Menu>();

    auto* application = m_menu->addSubmenu("CutefishSearch");
    application->setRole(cutefish::ui::MenuRole::Application);
    application->addItem("Show search", actions.show);
    application->addItem("Hide search", actions.hide);
    application->addSeparator();
    application->addItem("Settings", actions.settings, "Ctrl+,");
    application->addItem("Quit CutefishSearch", actions.quit);

    auto* search = m_menu->addSubmenu("Search");
    search->addItem("Clear query", actions.clear);
    search->addItem("Focus search", actions.show);

    auto* view = m_menu->addSubmenu("View");
    view->addItem("Show or hide search", actions.toggle);
}

MainMenu::~MainMenu() = default;

} // namespace CutefishSearch
