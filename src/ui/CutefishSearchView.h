#pragma once

#include <functional>
#include <string>

#include "base/signal.h"
#include "search/SearchController.h"
#include "ui/layout.h"

namespace CutefishSearch {

class ResultsView;
class SearchBox;

class CutefishSearchView : public cutefish::ui::Column
{
public:
    CutefishSearchView();

    void relayout() { layout(); }

    void focusSearch();
    void clearSearch();
    void setExpanded(bool expanded);
    void setPlaceholder(std::string text);

    std::function<void(bool)> onQueryChanged;
    std::function<void(bool)> onSearchFocusChanged;
    std::function<void()> onHideRequested;

private:
    SearchBox* m_search = nullptr;
    cutefish::ui::View* m_separator = nullptr;
    ResultsView* m_results = nullptr;
    SearchController m_searchController;
    cutefish::base::Subscription m_themeChanged;
};

} // namespace CutefishSearch
