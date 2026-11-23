#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "ui/list_view.h"

namespace cutefish::ui {
class Label;
class ThemeIcon;
} // namespace cutefish::ui

namespace CutefishSearch {

struct SearchResult;

// 検索結果を行狭義の ListView に描画する
class ResultsView : public cutefish::ui::ListView
{
public:
    ResultsView();

    void setResults(const std::vector<SearchResult>& results);
    void clearSelection();
    void moveSelection(int step);
    void activateSelected();

    std::function<void(size_t index)> onActivated;

private:
    std::vector<SearchResult> m_results;
    size_t m_selected = 0;
    bool m_hasSelection = false;
};

} // namespace CutefishSearch
