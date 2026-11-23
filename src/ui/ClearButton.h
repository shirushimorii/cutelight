#pragma once

#include <functional>
#include <optional>

#include "ui/color_mixer.h"
#include "ui/view.h"

namespace CutefishSearch {

// 入力があるときだけ表示される小さな消去ボタン
class ClearButton : public cutefish::ui::View
{
public:
    ClearButton();

    void setColor(cutefish::ui::ColorRecipe color);

    bool acceptsPointer() const override { return true; }

    cutefish::ui::SizeF sizeHint() const override;

    std::function<void()> onClicked;

private:
    std::optional<cutefish::ui::ColorRecipe> m_color;
    bool m_pressed = false;

protected:
    void paint(cutefish::ui::Painter& painter) override;
    void pointerPress(const cutefish::ui::PointerEvent& event) override;
    void pointerRelease(const cutefish::ui::PointerEvent& event) override;
};

} // namespace CutefishSearch
