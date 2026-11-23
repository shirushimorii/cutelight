#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "base/signal.h"

namespace cutefish::base {
class ApplicationRegistry;
class EventLoop;
struct DesktopEntry;
} // namespace cutefish::base

namespace CuteLight {

enum class SearchResultKind {
    Application,
    Calculator,
    Currency,
    WebSearch,
};

struct SearchResult
{
    SearchResultKind kind = SearchResultKind::Application;
    std::string title;
    std::string subtitle;
    std::string icon;
    std::string program;
    std::vector<std::string> arguments;
    std::string workingDirectory;
    std::string url;
    std::string value;
};

// 実際にインストールされているアプリケーションと電卓・換算・ウェブ検索を扱う
class SearchController
{
public:
    explicit SearchController(cutefish::base::EventLoop* loop);

    void setQuery(std::string query);
    const std::vector<SearchResult>& results() const { return m_results; }

    bool activate(size_t index) const;

private:
    void rebuild();
    static int score(const cutefish::base::DesktopEntry& entry, const std::string& query);

    cutefish::base::ApplicationRegistry* m_registry = nullptr;
    std::string m_query;
    std::string m_rawQuery;
    std::vector<SearchResult> m_results;
    cutefish::base::Subscription m_registryChanged;
};

} // namespace CuteLight
