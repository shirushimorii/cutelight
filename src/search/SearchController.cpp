#include "SearchController.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <map>
#include <optional>
#include <regex>
#include <sstream>
#include <utility>

#include "base/application_registry.h"
#include "base/desktop_entry.h"
#include "base/mime_apps.h"
#include "base/process.h"
#include "ui/clipboard.h"

namespace CuteLight {

namespace {

std::string lowered(std::string text)
{
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return text;
}

std::string trimmed(std::string text)
{
    const auto first = text.find_first_not_of(" \t\r\n");
    if (first == std::string::npos)
        return {};
    const auto last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

bool contains(const std::string& haystack, const std::string& needle)
{
    return haystack.find(needle) != std::string::npos;
}

std::string formatNumber(double value)
{
    if (!std::isfinite(value))
        return {};
    if (std::abs(value) < 1e15 && std::abs(value - std::round(value)) < 1e-9)
        return std::to_string(static_cast<long long>(std::round(value)));

    std::ostringstream out;
    out << std::setprecision(10) << value;
    std::string text = out.str();
    if (contains(text, ".")) {
        while (!text.empty() && text.back() == '0')
            text.pop_back();
        if (!text.empty() && text.back() == '.')
            text.pop_back();
    }
    return text;
}

bool isExpressionCandidate(const std::string& text)
{
    bool digit = false;
    bool symbol = false;
    for (unsigned char c : text) {
        if (std::isdigit(c))
            digit = true;
        else if (std::string("+-*/().").find(static_cast<char>(c)) != std::string::npos)
            symbol = true;
        else if (std::isspace(c))
            continue;
        else
            return false;
    }
    return digit && symbol;
}

// 四則演算と括弧だけを扱う小さな再下降パーサ
class ExpressionParser
{
public:
    explicit ExpressionParser(const std::string& text)
        : m_text(text)
    {
    }

    std::optional<double> parse()
    {
        auto value = expression();
        skipSpaces();
        if (!value || m_position != m_text.size())
            return std::nullopt;
        return value;
    }

private:
    void skipSpaces()
    {
        while (m_position < m_text.size() && std::isspace(static_cast<unsigned char>(m_text[m_position])))
            ++m_position;
    }

    std::optional<double> number()
    {
        skipSpaces();
        const size_t start = m_position;
        bool digits = false;
        while (m_position < m_text.size()) {
            const unsigned char c = static_cast<unsigned char>(m_text[m_position]);
            if (std::isdigit(c)) {
                digits = true;
                ++m_position;
            } else if (c == '.') {
                ++m_position;
            } else {
                break;
            }
        }
        if (!digits)
            return std::nullopt;
        try {
            return std::stod(m_text.substr(start, m_position - start));
        } catch (...) {
            return std::nullopt;
        }
    }

    std::optional<double> factor()
    {
        skipSpaces();
        if (m_position < m_text.size() && m_text[m_position] == '(') {
            ++m_position;
            auto value = expression();
            skipSpaces();
            if (m_position >= m_text.size() || m_text[m_position] != ')')
                return std::nullopt;
            ++m_position;
            return value;
        }
        return number();
    }

    std::optional<double> unary()
    {
        skipSpaces();
        if (m_position < m_text.size() && (m_text[m_position] == '-' || m_text[m_position] == '+')) {
            const bool negative = m_text[m_position] == '-';
            ++m_position;
            auto value = unary();
            if (!value)
                return std::nullopt;
            return negative ? -*value : *value;
        }
        return factor();
    }

    std::optional<double> term()
    {
        auto value = unary();
        if (!value)
            return std::nullopt;
        while (true) {
            skipSpaces();
            if (m_position >= m_text.size())
                break;
            const char op = m_text[m_position];
            if (op != '*' && op != '/')
                break;
            ++m_position;
            auto right = unary();
            if (!right)
                return std::nullopt;
            if (op == '*')
                *value *= *right;
            else {
                if (std::abs(*right) < 1e-12)
                    return std::nullopt;
                *value /= *right;
            }
        }
        return value;
    }

    std::optional<double> expression()
    {
        auto value = term();
        if (!value)
            return std::nullopt;
        while (true) {
            skipSpaces();
            if (m_position >= m_text.size())
                break;
            const char op = m_text[m_position];
            if (op != '+' && op != '-')
                break;
            ++m_position;
            auto right = term();
            if (!right)
                return std::nullopt;
            if (op == '+')
                *value += *right;
            else
                *value -= *right;
        }
        return value;
    }

    const std::string& m_text;
    size_t m_position = 0;
};

std::optional<SearchResult> makeCalculatorResult(const std::string& query)
{
    if (!isExpressionCandidate(query))
        return std::nullopt;
    const auto value = ExpressionParser(query).parse();
    if (!value)
        return std::nullopt;

    const std::string text = formatNumber(*value);
    if (text.empty())
        return std::nullopt;

    SearchResult result;
    result.kind = SearchResultKind::Calculator;
    result.title = text;
    result.subtitle = "Calculation";
    result.icon = "accessories-calculator";
    result.value = text;
    return result;
}

struct CurrencyRequest
{
    double amount = 0;
    std::string from;
    std::string to;
};

std::string normalizeCurrencyText(std::string text)
{
    const std::map<std::string, std::string> symbols {
        {"$", " usd "}, {"€", " eur "}, {"£", " gbp "}, {"¥", " jpy "}, {"₹", " inr "},
    };
    for (const auto& [symbol, code] : symbols) {
        size_t position = text.find(symbol);
        while (position != std::string::npos) {
            text.replace(position, symbol.size(), code);
            position = text.find(symbol, position + code.size());
        }
    }
    return lowered(trimmed(std::move(text)));
}

const std::map<std::string, double>& currencyRates()
{
    // 参考値。ネットワークを使わずに基本的な換算だけを提供する
    static const std::map<std::string, double> rates {
        {"usd", 1.0}, {"eur", 0.92}, {"gbp", 0.79}, {"jpy", 151.0}, {"cny", 7.25},
        {"krw", 1350.0}, {"inr", 83.5}, {"cad", 1.36}, {"aud", 1.52}, {"chf", 0.89},
    };
    return rates;
}

std::optional<CurrencyRequest> parseCurrencyRequest(const std::string& query)
{
    const std::string text = normalizeCurrencyText(query);
    static const std::regex pattern(
        R"(^([0-9]+(\.[0-9]+)?)\s*([a-z]{3})\s*(to|in|into|->|=)?\s*([a-z]{3})$)");
    std::smatch match;
    if (!std::regex_match(text, match, pattern))
        return std::nullopt;

    CurrencyRequest request;
    try {
        request.amount = std::stod(match[1].str());
    } catch (...) {
        return std::nullopt;
    }
    request.from = match[3].str();
    request.to = match[5].str();

    const auto& rates = currencyRates();
    if (!rates.contains(request.from) || !rates.contains(request.to))
        return std::nullopt;
    return request;
}

std::optional<SearchResult> makeCurrencyResult(const std::string& query)
{
    const auto request = parseCurrencyRequest(query);
    if (!request)
        return std::nullopt;

    const auto& rates = currencyRates();
    const double converted = request->amount / rates.at(request->from) * rates.at(request->to);
    const std::string source = formatNumber(request->amount) + " " + request->from;
    const std::string target = formatNumber(converted) + " " + request->to;

    SearchResult result;
    result.kind = SearchResultKind::Currency;
    result.title = target;
    result.subtitle = source + " converted";
    result.icon = "preferences-system";
    result.value = target;
    return result;
}

std::string urlEncode(const std::string& text)
{
    std::ostringstream out;
    for (unsigned char c : text) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            out << static_cast<char>(c);
        } else if (c == ' ') {
            out << '+';
        } else {
            out << '%' << std::uppercase << std::hex << std::setw(2) << std::setfill('0')
                << static_cast<int>(c) << std::dec;
        }
    }
    return out.str();
}

SearchResult makeWebResult(const std::string& query)
{
    SearchResult result;
    result.kind = SearchResultKind::WebSearch;
    result.title = "Search Google for \"" + query + "\"";
    result.subtitle = "Open in default browser";
    result.icon = "internet-web-browser";
    result.url = "https://www.google.com/search?q=" + urlEncode(query);
    return result;
}

} // namespace

SearchController::SearchController(cutefish::base::EventLoop* loop)
{
    auto& registry = cutefish::base::ApplicationRegistry::instance(loop);
    m_registry = &registry;
    m_registryChanged = registry.changed.connect([this] {
        if (!m_query.empty())
            rebuild();
    });
}

void SearchController::setQuery(std::string query)
{
    m_rawQuery = trimmed(std::move(query));
    m_query = lowered(m_rawQuery);
    rebuild();
}

int SearchController::score(const cutefish::base::DesktopEntry& entry, const std::string& query)
{
    if (query.empty())
        return 0;

    const std::string name = lowered(entry.name);
    const std::string generic = lowered(entry.genericName);
    const std::string comment = lowered(entry.comment);
    const std::string id = lowered(entry.id);

    int value = 0;
    if (name.starts_with(query))
        value = std::max(value, 100);
    if (generic.starts_with(query))
        value = std::max(value, 80);
    if (contains(name, query))
        value = std::max(value, 60);
    for (const auto& keyword : entry.keywords) {
        if (lowered(keyword) == query)
            value = std::max(value, 50);
        else if (contains(lowered(keyword), query))
            value = std::max(value, 40);
    }
    if (contains(comment, query))
        value = std::max(value, 30);
    if (contains(id, query))
        value = std::max(value, 20);
    return value;
}

void SearchController::rebuild()
{
    m_results.clear();
    if (m_query.empty())
        return;

    if (auto calculation = makeCalculatorResult(m_rawQuery)) {
        m_results.push_back(std::move(*calculation));
        return;
    }
    if (auto conversion = makeCurrencyResult(m_rawQuery)) {
        m_results.push_back(std::move(*conversion));
        return;
    }

    struct Candidate
    {
        int rank;
        const cutefish::base::DesktopEntry* entry;
    };
    std::vector<Candidate> candidates;

    if (m_registry) {
        for (const auto& entry : m_registry->applications()) {
            if (!entry || !entry->shouldShow())
                continue;
            const int rank = score(*entry, m_query);
            if (rank <= 0)
                continue;
            const auto command = entry->command();
            if (command.empty() || command.front().empty())
                continue;
            candidates.push_back({rank, entry.get()});
        }

        std::stable_sort(candidates.begin(), candidates.end(), [](const Candidate& a, const Candidate& b) {
            if (a.rank != b.rank)
                return a.rank > b.rank;
            return a.entry->name < b.entry->name;
        });
        if (candidates.size() > 12)
            candidates.resize(12);

        for (const auto& candidate : candidates) {
            const auto& entry = *candidate.entry;
            const auto command = entry.command();
            SearchResult result;
            result.kind = SearchResultKind::Application;
            result.title = entry.name.empty() ? entry.id : entry.name;
            result.subtitle = entry.comment.empty() ? entry.genericName : entry.comment;
            result.icon = entry.icon;
            result.program = command.front();
            result.arguments.assign(command.begin() + 1, command.end());
            result.workingDirectory = entry.workingDirectory;
            m_results.push_back(std::move(result));
        }
    }

    if (m_results.empty())
        m_results.push_back(makeWebResult(m_rawQuery));
}

bool SearchController::activate(size_t index) const
{
    if (index >= m_results.size())
        return false;
    const auto& result = m_results[index];

    switch (result.kind) {
    case SearchResultKind::Application:
        return cutefish::base::Process::startDetached(result.program, result.arguments,
                                                       result.workingDirectory) > 0;
    case SearchResultKind::WebSearch:
        if (result.url.empty())
            return false;
        cutefish::base::mimeapps::openUri(result.url, {}, [](bool) {});
        return true;
    case SearchResultKind::Calculator:
    case SearchResultKind::Currency:
        if (result.value.empty())
            return false;
        cutefish::ui::Clipboard::setText(result.value);
        return true;
    }
    return false;
}

} // namespace CuteLight
