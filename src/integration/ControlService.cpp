#include "ControlService.h"

#include "base/dbus.h"

namespace CutefishSearch {

ControlService::ControlService(cutefish::base::dbus::Connection* bus,
                               std::function<void()> show,
                               std::function<void()> hide,
                               std::function<void()> toggle,
                               std::function<void()> showSettings)
{
    if (!bus)
        return;

    bus->requestName("org.cutefish.Search");
    m_object = std::make_unique<cutefish::base::dbus::Object>(
        bus, "/org/cutefish/Search", "org.cutefish.Search");
    m_object->addMethod("Show", [show] {
        show();
        return true;
    });
    m_object->addMethod("Hide", [hide] {
        hide();
        return true;
    });
    m_object->addMethod("Toggle", [toggle] {
        toggle();
        return true;
    });
    m_object->addMethod("ShowSettings", [showSettings] {
        showSettings();
        return true;
    });
    m_object->publish();
}

ControlService::~ControlService() = default;

} // namespace CutefishSearch
