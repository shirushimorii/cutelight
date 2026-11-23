#include "ResultsView.h"

#include <algorithm>
#include <utility>

#include "ui/color_id.h"
#include "ui/color_mixer.h"
#include "ui/event.h"
#include "ui/icon.h"
#include "ui/label.h"
#include "ui/painter.h"

#include "DesignTokens.h"
#include "search/SearchController.h"

namespace CutefishSearch {

namespace {

class ResultRow final : public cutefish::ui::View
{
public:
    ResultRow()
    {
        setName("ResultRow");
        m_icon = addChild<cutefish::ui::ThemeIcon>();
        m_icon->setSize(28);
        m_title = addChild<cutefish::ui::Label>();
        m_subtitle = addChild<cutefish::ui::Label>();

        m_title->setFontSize(15);
        m_title->setWeight(500);
        m_title->setElide(cutefish::ui::Label::Elide::Right);
        m_subtitle->setFontSize(12);
        m_subtitle->setElide(cutefish::ui::Label::Elide::Right);
        m_subtitle->setTextAlignment(cutefish::ui::Label::TextAlignment::Right);
        m_subtitle->setColor(cutefish::ui::ColorRecipe(cutefish::ui::ColorId::DisabledText));

        cutefish::ui::LayoutHints iconHints;
        iconHints.alignment = cutefish::ui::Alignment::Center;
        iconHints.preferredWidth = 28;
        m_icon->setLayoutHints(iconHints);

        cutefish::ui::LayoutHints textHints;
        textHints.fillWidth = true;
        m_title->setLayoutHints(textHints);
        m_subtitle->setLayoutHints(textHints);
    }

    void setResult(const SearchResult& result)
    {
        m_icon->setIconName(result.icon);
        m_title->setText(result.title);
        m_subtitle->setText(result.subtitle);
    }

    void setSelected(bool selected)
    {
        if (m_selected == selected)
            return;
        m_selected = selected;
        requestPaint();
    }

    bool acceptsPointer() const override { return true; }

    std::function<void()> onActivated;

protected:
    void layout() override
    {
        const auto rect = geometry();
        const double textY = (rect.height - 21) / 2;
        const double titleWidth = std::min<double>(260, rect.width * 0.45);
        const double subtitleX = 52 + titleWidth + 12;
        m_icon->setGeometry({12, (rect.height - 28) / 2, 28, 28});
        m_title->setGeometry({52, textY, titleWidth, 21});
        m_subtitle->setGeometry({subtitleX, textY, std::max<double>(0, rect.width - 12 - subtitleX), 21});
    }

    void paint(cutefish::ui::Painter& painter) override
    {
        if (!m_selected)
            return;
        const auto color = cutefish::ui::ColorRecipe(cutefish::ui::ColorId::Highlight).resolve();
        painter.save();
        painter.multiplyOpacity(0.22);
        painter.fillRoundedRect(localRect(), 10, color);
        painter.restore();
    }

    void pointerPress(const cutefish::ui::PointerEvent&) override
    {
        m_pressed = true;
    }

    void pointerRelease(const cutefish::ui::PointerEvent&) override
    {
        const bool clicked = m_pressed;
        m_pressed = false;
        if (clicked && onActivated)
            onActivated();
    }

private:
    cutefish::ui::ThemeIcon* m_icon = nullptr;
    cutefish::ui::Label* m_title = nullptr;
    cutefish::ui::Label* m_subtitle = nullptr;
    bool m_selected = false;
    bool m_pressed = false;
};

} // namespace

ResultsView::ResultsView()
{
    setName("ResultsView");
    setRowHeight(Tokens::Results::RowHeight);
    setVerticalBarPolicy(cutefish::ui::ScrollView::BarPolicy::AlwaysOff);
    setBarsBelowContent(true);

    createItem = [] {
        return std::make_unique<ResultRow>();
    };
    bindItem = [this](cutefish::ui::View* item, size_t row) {
        auto* resultRow = static_cast<ResultRow*>(item);
        if (row >= m_results.size())
            return;
        resultRow->setResult(m_results[row]);
        resultRow->setSelected(m_hasSelection && row == m_selected);
        resultRow->onActivated = [this, row] {
            m_selected = row;
            m_hasSelection = true;
            activateSelected();
        };
    };
}

void ResultsView::setResults(const std::vector<SearchResult>& results)
{
    m_results = results;
    m_selected = 0;
    m_hasSelection = false;
    setRowCount(m_results.size());
    reload();
}

void ResultsView::clearSelection()
{
    if (!m_hasSelection && m_selected == 0)
        return;
    m_selected = 0;
    m_hasSelection = false;
    reload();
}

void ResultsView::moveSelection(int step)
{
    if (m_results.empty())
        return;
    if (!m_hasSelection) {
        m_selected = step > 0 ? 0 : m_results.size() - 1;
    } else {
        const long long next = static_cast<long long>(m_selected) + step;
        if (next < 0)
            m_selected = m_results.size() - 1;
        else if (next >= static_cast<long long>(m_results.size()))
            m_selected = 0;
        else
            m_selected = static_cast<size_t>(next);
    }
    m_hasSelection = true;
    reload();
    scrollToRow(m_selected);
}

void ResultsView::activateSelected()
{
    if (!m_hasSelection || m_selected >= m_results.size() || !onActivated)
        return;
    onActivated(m_selected);
}

} // namespace CutefishSearch
