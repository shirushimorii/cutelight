#pragma once

#include "ui/geometry.h"

namespace CuteLight::Tokens {

namespace Window {
inline constexpr double PreferredWidth = 760.0;
inline constexpr double MinimumWidth = 480.0;
inline constexpr double MaximumWidth = 900.0;
inline constexpr double CollapsedHeight = 76.0;
inline constexpr double ExpandedHeight = 430.0;
inline constexpr double MaximumHeight = 620.0;
inline constexpr double Radius = 20.0;
inline constexpr double BlurRadius = 22.0;
inline constexpr double VerticalPosition = 0.37;
inline constexpr double EdgeMargin = 16.0;
} // namespace Window

namespace Search {
inline constexpr double Height = 76.0;
inline constexpr double HorizontalPadding = 24.0;
inline constexpr double IconSize = 22.0;
inline constexpr double IconTextSpacing = 16.0;
inline constexpr double ClearButtonSize = 28.0;
inline constexpr double FontSize = 22.0;
inline constexpr int FontWeight = 400;
inline constexpr double Radius = 14.0;
} // namespace Search

namespace Results {
inline constexpr double TopPadding = 8.0;
inline constexpr double HorizontalPadding = 12.0;
inline constexpr double BottomPadding = 12.0;
inline constexpr double RowHeight = 44.0;
} // namespace Results

namespace Animation {
inline constexpr int OpenMs = 190;
inline constexpr int CloseMs = 140;
inline constexpr int ExpandMs = 200;
} // namespace Animation

namespace Palette {
inline cutefish::ui::Color darkSurface(double alpha = 1.0)
{
    return cutefish::ui::Color::rgb(0x242428).withAlpha(alpha);
}

inline cutefish::ui::Color lightSurface(double alpha = 1.0)
{
    return cutefish::ui::Color::rgb(0xF4F4F7).withAlpha(alpha);
}

inline cutefish::ui::Color primaryText(bool dark)
{
    return cutefish::ui::Color::rgb(dark ? 0xF5F5F7 : 0x202024);
}

inline cutefish::ui::Color secondaryText(bool dark)
{
    return cutefish::ui::Color::rgb(dark ? 0xA5A5AE : 0x777780);
}

inline cutefish::ui::Color separator(bool dark)
{
    return cutefish::ui::Color::fromArgb(dark ? 0x14FFFFFF : 0x12000000);
}
} // namespace Palette

} // namespace CuteLight::Tokens
