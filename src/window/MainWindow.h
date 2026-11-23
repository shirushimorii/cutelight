#pragma once

#include <chrono>
#include <functional>
#include <memory>

#include "base/signal.h"
#include "settings/Settings.h"
#include "ui/geometry.h"
#include "ui/window.h"

namespace cutefish::ui {
class Animation;
}

namespace CutefishSearch {

class CutefishSearchView;

class MainWindow : public cutefish::ui::Window
{
public:
    explicit MainWindow(const SettingsData& settings);
    ~MainWindow() override;

    void showCentered();
    void hideVisually();
    bool isVisible() const { return m_visible; }
    void toggle();
    void clearSearch();
    void focusSearch();
    void applySettings(const SettingsData& settings);

    std::function<void()> onHidden;
    std::function<void()> onOpenSettings;
    std::function<void()> onFocusLost;
    std::function<void(bool)> onSearchFocusChanged;

private:
    void centerOnScreen();
    void updateAppearance();
    void applyBackdrop();
    cutefish::ui::GlassMaterial glassMaterial() const;
    void animateTo(double target);
    void animateExpansion(bool expanded);
    void closeAnimated();

    CutefishSearchView* m_view = nullptr;
    std::unique_ptr<cutefish::ui::Animation> m_animation;
    std::unique_ptr<cutefish::ui::Animation> m_sizeAnimation;
    cutefish::base::Subscription m_themeChanged;

    SettingsData m_settings;
    cutefish::ui::PointF m_topLeft;
    double m_positionRatio = 0.37;
    double m_windowWidth = 0;
    double m_expandedHeight = 0;
    double m_animationFrom = 0;
    double m_animationTo = 1;
    double m_heightFrom = 0;
    double m_heightTo = 0;
    bool m_closing = false;
    bool m_visible = false;
    bool m_compositorGlass = false;
};

} // namespace CutefishSearch
