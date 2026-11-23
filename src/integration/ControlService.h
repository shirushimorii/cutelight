#pragma once

#include <functional>
#include <memory>

namespace cutefish::base::dbus {
class Connection;
class Object;
} // namespace cutefish::base::dbus

namespace CutefishSearch {

// ランチャーや手動割り当てのショートカットから使う D-Bus 制御インターフェイス
class ControlService
{
public:
    ControlService(cutefish::base::dbus::Connection* bus,
                   std::function<void()> show,
                   std::function<void()> hide,
                   std::function<void()> toggle,
                   std::function<void()> showSettings);
    ~ControlService();

private:
    std::unique_ptr<cutefish::base::dbus::Object> m_object;
};

} // namespace CutefishSearch
