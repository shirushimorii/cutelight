#include "CuteLightView.h"

#include <utility>

#include "ui/application.h"
#include "ui/color_id.h"
#include "ui/color_mixer.h"
#include "ui/event.h"
#include "ui/keys.h"
#include "ui/layout.h"
#include "ui/painter.h"
#include "ui/theme.h"

#include "DesignTokens.h"
#include "ResultsView.h"
#include "SearchBox.h"

namespace CuteLight {

namespace {

class Separator final : public cutefish::ui::View
{
public:
    Separator()
    {
        setName("SearchSeparator");
    }

    cutefish::ui::SizeF sizeHint() const override
    {
        return {0, 1};
    }

protected:
    void paint(cutefish::ui::Painter& painter) override
    {
        const bool dark = cutefish::ui::Theme::current().darkMode;
        const auto color = cutefish::ui::ColorRecipe(Tokens::Palette::separator(dark)).resolve();
        painter.drawHorizontalLine(0, 0, geometry().width, color, 1);
    }
};

} // namespace

CuteLightView::CuteLightView()
    : m_searchController(&cutefish::ui::Application::instance()->loop())
{
    setName("CuteLightView");
    setClipsChildren(true);
    setChildClipRadius(Tokens::Window::Radius);
    setSpacing(0);

    m_search = addChild<SearchBox>();
    cutefish::ui::LayoutHints searchHints;
    searchHints.fillWidth = true;
    searchHints.preferredHeight = Tokens::Search::Height;
    m_search->setLayoutHints(searchHints);
    m_search->onTextChanged = [this] {
        const std::string query = m_search->text();
        m_searchController.setQuery(query);
        m_results->setResults(m_searchController.results());
        if (onQueryChanged)
            onQueryChanged(!query.empty());
    };
    m_search->onSpecialKey = [this](uint32_t key) {
        using namespace cutefish::ui;
        if (key == key::Escape) {
            if (onHideRequested)
                onHideRequested();
            // ウィンドウのショートカットにも同じクローズ処理を任せる
            return false;
        }
        if (key == key::Up) {
            m_results->moveSelection(-1);
            return true;
        }
        if (key == key::Down) {
            m_results->moveSelection(1);
            return true;
        }
        if (key == key::Return && m_results) {
            m_results->activateSelected();
            return true;
        }
        return false;
    };
    m_search->onFocusChanged = [this](bool focused) {
        if (onSearchFocusChanged)
            onSearchFocusChanged(focused);
    };

    m_separator = addChild<Separator>();
    cutefish::ui::LayoutHints separatorHints;
    separatorHints.fillWidth = true;
    separatorHints.preferredHeight = 1;
    m_separator->setLayoutHints(separatorHints);
    m_separator->setVisible(false);

    m_results = addChild<ResultsView>();
    cutefish::ui::LayoutHints resultHints;
    resultHints.fillWidth = true;
    resultHints.fillHeight = true;
    m_results->setLayoutHints(resultHints);
    m_results->setVisible(false);
    m_results->onActivated = [this](size_t index) {
        if (m_searchController.activate(index) && onHideRequested)
            onHideRequested();
    };

    // テーマが初期化後に切り替わっても固定カラーを追従させる
    m_themeChanged = cutefish::ui::Theme::changed().connect([this] {
        if (m_search)
            m_search->updateTheme();
        if (m_separator)
            m_separator->requestPaint();
    });
}

void CuteLightView::focusSearch()
{
    if (m_search)
        m_search->focus();
}

void CuteLightView::clearSearch()
{
    if (m_search)
        m_search->clear();
}

void CuteLightView::setExpanded(bool expanded)
{
    if (m_separator)
        m_separator->setVisible(expanded);
    if (m_results)
        m_results->setVisible(expanded);
    requestLayout();
}

void CuteLightView::setPlaceholder(std::string text)
{
    if (m_search)
        m_search->setPlaceholder(std::move(text));
}

} // namespace CuteLight
