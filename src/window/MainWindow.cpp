#include "MainWindow.h"

#include <algorithm>
#include <cmath>
#include <optional>

#include "ui/animation.h"
#include "ui/application.h"
#include "ui/color_mixer.h"
#include "ui/keys.h"
#include "ui/theme.h"

#include "ui/CuteLightView.h"
#include "ui/DesignTokens.h"

namespace CuteLight {

MainWindow::MainWindow(const SettingsData& settings)
    : m_settings(settings)
    , m_positionRatio(Settings::positionRatio(settings.position))
{
    setTitle("CuteLight");
    setSkipTaskbar(true);
    setUserResizable(false);
    setLevel(cutefish::ui::WindowLevel::Floating);
    setCastsShadow(true);
    setResizeAroundCenter(false);
    setMinimumSize({Tokens::Window::MinimumWidth, Tokens::Window::CollapsedHeight});
    setMaximumSize({Tokens::Window::MaximumWidth, Tokens::Window::MaximumHeight});
    resize({Tokens::Window::PreferredWidth, Tokens::Window::CollapsedHeight});

    updateAppearance();

    auto view = std::make_unique<CuteLightView>();
    m_view = view.get();
    setContentView(std::move(view));
    m_view->setPlaceholder(m_settings.placeholder);

    m_animation = std::make_unique<cutefish::ui::Animation>(this);
    m_animation->setDuration(std::chrono::milliseconds(Tokens::Animation::OpenMs));
    m_animation->setEasing(cutefish::ui::Animation::Easing::OutCubic);
    m_animation->onUpdate = [this](double value) {
        const double progress = m_animationFrom + (m_animationTo - m_animationFrom) * value;
        setOpacity(progress);
        if (m_view)
            m_view->setTransform({{0, -4.0 * (1.0 - progress)}, 0.985 + 0.015 * progress});
    };
    m_animation->onFinished = [this] {
        if (!m_closing)
            return;
        hideVisually();
        m_closing = false;
        if (onHidden)
            onHidden();
    };

    m_sizeAnimation = std::make_unique<cutefish::ui::Animation>(this);
    m_sizeAnimation->setDuration(std::chrono::milliseconds(Tokens::Animation::ExpandMs));
    m_sizeAnimation->setEasing(cutefish::ui::Animation::Easing::OutCubic);
    m_sizeAnimation->onUpdate = [this](double value) {
        const double height = m_heightFrom + (m_heightTo - m_heightFrom) * value;
        resize({m_windowWidth, height});
        applyBackdrop();
    };

    m_view->onQueryChanged = [this](bool hasQuery) {
        animateExpansion(hasQuery);
    };
    m_view->onHideRequested = [this] {
        closeAnimated();
    };
    m_view->onSearchFocusChanged = [this](bool focused) {
        if (onSearchFocusChanged)
            onSearchFocusChanged(focused);
    };

    addShortcut(cutefish::ui::NoModifier, cutefish::ui::key::Escape, [this] {
        closeAnimated();
    });
    addShortcut(cutefish::ui::ControlModifier, ',', [this] {
        if (onOpenSettings)
            onOpenSettings();
    });

    onCloseRequested = [this] {
        closeAnimated();
    };

    onActiveChanged = [this] {
        if (isActive()) {
            // ポップアップ表示時は必ず入力欄へフォーカスを戻す
            if (m_view && !m_closing)
                m_view->focusSearch();
            return;
        }
        // 別の場所をクリックして非アクティブになったら検索バーを閉じる
        if (m_visible && !m_closing && onFocusLost)
            onFocusLost();
    };

    m_themeChanged = cutefish::ui::Theme::changed().connect([this] {
        updateAppearance();
    });
}

MainWindow::~MainWindow() = default;

void MainWindow::showCentered()
{
    if (m_animation->isRunning())
        m_animation->stop();
    if (m_sizeAnimation->isRunning())
        m_sizeAnimation->stop();

    m_closing = false;
    m_visible = true;
    setInputTransparent(false);
    setOpacity(0);
    if (m_view) {
        m_view->clearSearch();
        m_view->setExpanded(false);
        m_view->setTransform({{0, -4}, 0.985});
    }

    centerOnScreen();
    resize({m_windowWidth, Tokens::Window::CollapsedHeight});

    // 通常のトップレベルは compositor が中央配置するため、下端基準の
    // 配置アンカーに目的の左上座標とウィンドウ高さを渡して上端を固定する
    cutefish::ui::PlacementAnchor anchor;
    anchor.rect = {m_topLeft.x, m_topLeft.y + Tokens::Window::CollapsedHeight, 1, 1};
    anchor.anchor = cutefish::ui::PopupAnchor::TopLeft;
    anchor.gravity = cutefish::ui::PopupAnchor::TopLeft;
    setPlacementAnchor(anchor);
    setScreenPosition(m_topLeft);
    updateAppearance();
    show();
    activate();

    m_compositorGlass = backgroundGlassAvailable();
    updateAppearance();

    if (m_view)
        m_view->focusSearch();
    activate();

    animateTo(1);
}

void MainWindow::hideVisually()
{
    m_visible = false;
    // Wayland のトップレベルは hide() だけでは surface が残るため、
    // 完全に透明にしてクリックも通さないようにする
    setOpacity(0);
    setInputTransparent(true);
    hide();
    if (m_view)
        m_view->setTransform({});
}

void MainWindow::toggle()
{
    if (!m_visible)
        showCentered();
    else
        closeAnimated();
}

void MainWindow::clearSearch()
{
    if (m_view)
        m_view->clearSearch();
}

void MainWindow::focusSearch()
{
    if (m_view)
        m_view->focusSearch();
}

void MainWindow::applySettings(const SettingsData& settings)
{
    const bool positionChanged = m_settings.position != settings.position;
    m_settings = settings;
    m_positionRatio = Settings::positionRatio(settings.position);
    if (m_view)
        m_view->setPlaceholder(m_settings.placeholder);
    updateAppearance();

    if (positionChanged && !isHidden()) {
        centerOnScreen();

        // 配置は表示中でもコンポジタのアンカーで固定する
        cutefish::ui::PlacementAnchor anchor;
        anchor.rect = {m_topLeft.x, m_topLeft.y + Tokens::Window::CollapsedHeight, 1, 1};
        anchor.anchor = cutefish::ui::PopupAnchor::TopLeft;
        anchor.gravity = cutefish::ui::PopupAnchor::TopLeft;
        setPlacementAnchor(anchor);
        applyBackdrop();
    }
}

void MainWindow::centerOnScreen()
{
    using namespace cutefish::ui;

    const auto screens = Application::instance()->screens();
    const Application::Screen* target = nullptr;
    for (const Application::Screen& screen : screens) {
        if (screen.primary) {
            target = &screen;
            break;
        }
    }
    if (!target && !screens.empty())
        target = &screens.front();

    RectF area {0, 0, 1920, 1080};
    if (target) {
        area = (target->workArea.width > 0 && target->workArea.height > 0)
            ? target->workArea
            : target->geometry;
    }

    const double usableWidth = std::max<double>(Tokens::Window::MinimumWidth,
                                                area.width - Tokens::Window::EdgeMargin * 2);
    const double usableHeight = std::max<double>(Tokens::Window::CollapsedHeight,
                                                 area.height - Tokens::Window::EdgeMargin * 2);
    const double maxWidth = std::min(Tokens::Window::MaximumWidth, usableWidth);
    const double minWidth = std::min(Tokens::Window::MinimumWidth, maxWidth);
    m_windowWidth = std::clamp(Tokens::Window::PreferredWidth, minWidth, maxWidth);

    const double maxHeight = std::min(Tokens::Window::MaximumHeight, usableHeight);
    m_expandedHeight = std::clamp(Tokens::Window::ExpandedHeight,
                                  std::min(Tokens::Window::CollapsedHeight, maxHeight), maxHeight);

    const double x = area.x + (area.width - m_windowWidth) / 2;
    double y = area.y + area.height * m_positionRatio;
    if (y + m_expandedHeight > area.bottom())
        y = area.bottom() - m_expandedHeight;
    if (y < area.y)
        y = area.y;

    m_topLeft = {x, y};
}

void MainWindow::updateAppearance()
{
    using namespace cutefish::ui;

    const bool dark = Theme::current().darkMode;
    const Color fallback = dark
        ? Tokens::Palette::darkSurface(0.86)
        : Tokens::Palette::lightSurface(0.90);
    setBackgroundColor(ColorRecipe(m_compositorGlass ? Color::fromArgb(0x00000000) : fallback));
    applyBackdrop();
}

void MainWindow::applyBackdrop()
{
    using namespace cutefish::ui;

    const RectF rect {0, 0, size().width, size().height};
    if (m_view && m_view->geometry() != rect) {
        m_view->setGeometry(rect);
        m_view->relayout();
    }
    setBackgroundBlur(rect, Tokens::Window::BlurRadius);
    if (m_compositorGlass)
        setBackgroundGlass(glassMaterial());
    else
        setBackgroundGlass(std::nullopt);
}

cutefish::ui::GlassMaterial MainWindow::glassMaterial() const
{
    using namespace cutefish::ui;

    const bool dark = Theme::current().darkMode;
    const bool tinted = m_settings.appearance == AppearanceMode::Tinted;
    const double strength = std::clamp(m_settings.tint / 100.0, 0.0, 1.0);
    const Color surface = dark ? Tokens::Palette::darkSurface() : Tokens::Palette::lightSurface();
    GlassMaterial material;
    material.blurSigma = tinted ? 10.0 : 16.0;
    material.bevel = tinted ? 10.0 : 8.0;
    material.depth = tinted ? 14.0 : 8.0;
    material.ior = tinted ? 1.5 : 1.2;
    material.profile = tinted ? 2.2 : 1.6;
    material.displacement = tinted ? 18.0 : 6.0;
    material.chroma = tinted ? 0.08 : 0.0;
    material.lightAngle = 55.0;
    material.specularIntensity = tinted ? 0.08 : 0.12;
    material.shininess = 60.0;
    material.sheen = tinted ? 0.04 : 0.06;
    material.ambientOcclusion = 0.12;
    material.ambientOcclusionRadius = 6.0;
    material.saturation = tinted ? 0.70 : 0.40;
    material.tintRed = surface.r / 255.0;
    material.tintGreen = surface.g / 255.0;
    material.tintBlue = surface.b / 255.0;
    material.tintAlpha = tinted ? 0.35 + 0.60 * strength : 0.10 + 0.80 * strength;
    material.rim = tinted ? (dark ? 0.12 : 0.18) : (dark ? 0.08 : 0.14);
    return material;
}

void MainWindow::animateTo(double target)
{
    m_animationFrom = target > 0.5 ? 0 : 1;
    m_animationTo = target;
    m_animation->start();
}

void MainWindow::animateExpansion(bool expanded)
{
    if (!m_view || isHidden())
        return;

    m_view->setExpanded(expanded);
    const double target = expanded ? m_expandedHeight : Tokens::Window::CollapsedHeight;
    if (std::abs(size().height - target) < 0.5)
        return;

    m_heightFrom = size().height;
    m_heightTo = target;
    if (m_sizeAnimation->isRunning())
        m_sizeAnimation->stop();
    m_sizeAnimation->start();
}

void MainWindow::closeAnimated()
{
    if (m_closing || isHidden())
        return;

    m_closing = true;
    m_animation->setDuration(std::chrono::milliseconds(Tokens::Animation::CloseMs));
    m_animation->setEasing(cutefish::ui::Animation::Easing::InQuad);
    animateTo(0);
}

} // namespace CuteLight
