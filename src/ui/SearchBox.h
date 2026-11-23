#pragma once

#include <cstdint>
#include <functional>
#include <string>

#include "ui/layout.h"

namespace cutefish::ui {
class Icon;
class Row;
class TextField;
} // namespace cutefish::ui

namespace CuteLight {

class ClearButton;

// 検索アイコン、入力欄、消去ボタンを同じ軸に並べる検索行
class SearchBox : public cutefish::ui::Row
{
public:
    SearchBox();

    const std::string& text() const;
    void setPlaceholder(std::string text);
    void clear();
    void focus();

    void updateTheme();

    cutefish::ui::SizeF sizeHint() const override;

    std::function<void()> onTextChanged;
    std::function<bool(uint32_t key)> onSpecialKey;
    std::function<void(bool)> onFocusChanged;

private:
    void updateClearButton();

    cutefish::ui::Icon* m_icon = nullptr;
    cutefish::ui::TextField* m_text = nullptr;
    ClearButton* m_clear = nullptr;
};

} // namespace CuteLight
