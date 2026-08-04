#include <winsock2.h>
#include <ws2tcpip.h>
#include <wininet.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <locale>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wininet.lib")

struct UrlParts {
    std::string scheme;
    std::string host;
    std::string path;
    std::string query;
};

void configureConsole() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    std::setlocale(LC_ALL, ".UTF-8");
}

std::string trim(const std::string& s) {
    const auto begin = std::find_if_not(s.begin(), s.end(), [](unsigned char c) { return std::isspace(c) != 0; });
    const auto end = std::find_if_not(s.rbegin(), s.rend(), [](unsigned char c) { return std::isspace(c) != 0; }).base();
    return (begin >= end) ? "" : std::string(begin, end);
}

std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

std::string decodeHtmlEntities(const std::string& text) {
    std::string result = text;
    const std::vector<std::pair<std::string, std::string>> replacements = {
        {"&nbsp;", " "}, {"&amp;", "&"}, {"&lt;", "<"}, {"&gt;", ">"},
        {"&quot;", "\""}, {"&#39;", "'"}
    };

    for (const auto& item : replacements) {
        size_t pos = 0;
        while ((pos = result.find(item.first, pos)) != std::string::npos) {
            result.replace(pos, item.first.size(), item.second);
            pos += item.second.size();
        }
    }

    return result;
}

std::string collapseWhitespace(const std::string& text) {
    std::string result;
    bool previousSpace = false;

    for (unsigned char ch : text) {
        if (std::isspace(ch) != 0) {
            if (!result.empty() && !previousSpace) {
                result.push_back(' ');
                previousSpace = true;
            }
        } else {
            result.push_back(static_cast<char>(ch));
            previousSpace = false;
        }
    }

    return trim(result);
}

std::string stripHtmlTags(const std::string& html) {
    std::string result;
    bool insideTag = false;
    bool skipBlock = false;
    std::string blockTag;

    for (size_t i = 0; i < html.size(); ++i) {
        if (html[i] == '<') {
            size_t end = html.find('>', i + 1);
            if (end == std::string::npos) {
                break;
            }

            std::string tag = toLower(html.substr(i + 1, end - i - 1));
            if (tag.find("/script") == 0 || tag.find("/style") == 0) {
                skipBlock = false;
                blockTag.clear();
                insideTag = true;
                continue;
            }

            if (tag.find("script") == 0 || tag.find("style") == 0) {
                skipBlock = true;
                blockTag = tag;
                insideTag = true;
                continue;
            }

            insideTag = true;
            continue;
        }

        if (html[i] == '>') {
            insideTag = false;
            continue;
        }

        if (!insideTag && !skipBlock) {
            result.push_back(html[i]);
        }
    }

    return decodeHtmlEntities(collapseWhitespace(result));
}

std::string extractTitle(const std::string& html) {
    const std::string lower = toLower(html);
    const size_t start = lower.find("<title>");
    if (start == std::string::npos) {
        return "No title";
    }

    const size_t end = lower.find("</title>", start);
    if (end == std::string::npos) {
        return "No title";
    }

    return collapseWhitespace(stripHtmlTags(html.substr(start + 7, end - start - 7)));
}

std::string makeSummary(const std::string& html, const std::string& url) {
    std::string text = stripHtmlTags(html);
    std::string title = extractTitle(html);

    if (text.empty()) {
        return "URL: " + url + "\nTitle: " + title + "\nSummary: No readable content found.";
    }

    std::string preview = text;
    if (preview.size() > 220) {
        preview = preview.substr(0, 220);
        const size_t lastSpace = preview.find_last_of(' ');
        if (lastSpace != std::string::npos) {
            preview = preview.substr(0, lastSpace);
        }
        preview += "...";
    }

    std::vector<std::string> words;
    std::stringstream ss(text);
    std::string token;
    while (ss >> token) {
        if (token.size() >= 4) {
            words.push_back(token);
        }
    }

    std::string keywords;
    const size_t limit = std::min<size_t>(6, words.size());
    for (size_t i = 0; i < limit; ++i) {
        if (!keywords.empty()) {
            keywords += ", ";
        }
        keywords += words[i];
    }

    std::ostringstream out;
    out << "URL: " << url << "\n"
        << "Title: " << title << "\n"
        << "Summary: " << preview << "\n"
        << "Keywords: " << (keywords.empty() ? "n/a" : keywords);
    return out.str();
}

UrlParts parseUrl(const std::string& url) {
    UrlParts parts;
    const size_t schemePos = url.find("://");
    if (schemePos == std::string::npos) {
        throw std::runtime_error("Invalid URL: missing scheme");
    }

    parts.scheme = url.substr(0, schemePos);
    std::string rest = url.substr(schemePos + 3);

    const size_t pathPos = rest.find('/');
    const size_t queryPos = rest.find('?');
    size_t hostEnd = rest.size();

    if (pathPos != std::string::npos && (queryPos == std::string::npos || pathPos < queryPos)) {
        hostEnd = pathPos;
    } else if (queryPos != std::string::npos) {
        hostEnd = queryPos;
    }

    parts.host = rest.substr(0, hostEnd);

    if (pathPos != std::string::npos) {
        size_t pathEnd = rest.size();
        if (queryPos != std::string::npos && queryPos > pathPos) {
            pathEnd = queryPos;
        }
        parts.path = rest.substr(pathPos, pathEnd - pathPos);
        if (parts.path.empty()) {
            parts.path = "/";
        }
    } else {
        parts.path = "/";
    }

    if (queryPos != std::string::npos) {
        parts.query = rest.substr(queryPos + 1);
    }

    return parts;
}

std::string joinUrl(const UrlParts& parts) {
    std::string result = parts.scheme + "://" + parts.host + parts.path;
    if (!parts.query.empty()) {
        result += "?" + parts.query;
    }
    return result;
}

std::string normalizeUrl(const std::string& input) {
    std::string url = trim(input);
    if (url.empty()) {
        return url;
    }

    if (url.rfind("http://", 0) == 0 || url.rfind("https://", 0) == 0) {
        return url;
    }

    if (url.find("://") != std::string::npos) {
        return url;
    }

    return "http://" + url;
}

std::string resolveUrl(const std::string& baseUrl, const std::string& href) {
    if (href.empty()) {
        return "";
    }

    if (href.rfind("http://", 0) == 0 || href.rfind("https://", 0) == 0) {
        return href;
    }

    if (href[0] == '#') {
        return baseUrl;
    }

    const UrlParts base = parseUrl(baseUrl);

    if (href[0] == '/') {
        return base.scheme + "://" + base.host + href;
    }

    if (href.rfind("mailto:", 0) == 0 || href.rfind("javascript:", 0) == 0) {
        return "";
    }

    std::string basePath = base.path;
    if (basePath.empty()) {
        basePath = "/";
    }
    const size_t lastSlash = basePath.find_last_of('/');
    if (lastSlash != std::string::npos && lastSlash != 0) {
        basePath = basePath.substr(0, lastSlash + 1);
    } else {
        basePath = "/";
    }

    return base.scheme + "://" + base.host + basePath + href;
}

std::vector<std::string> extractLinks(const std::string& html, const std::string& baseUrl) {
    std::vector<std::string> links;
    std::string lower = toLower(html);
    size_t pos = 0;

    while ((pos = lower.find("<a", pos)) != std::string::npos) {
        const size_t endTag = lower.find('>', pos);
        if (endTag == std::string::npos) {
            break;
        }

        const std::string tag = html.substr(pos, endTag - pos);
        const std::string tagLower = toLower(tag);
        const size_t hrefPos = tagLower.find("href");
        if (hrefPos == std::string::npos) {
            pos = endTag + 1;
            continue;
        }

        const size_t eqPos = tag.find('=', hrefPos);
        if (eqPos == std::string::npos) {
            pos = endTag + 1;
            continue;
        }

        size_t start = eqPos + 1;
        while (start < tag.size() && std::isspace(static_cast<unsigned char>(tag[start])) != 0) {
            ++start;
        }

        if (start >= tag.size()) {
            pos = endTag + 1;
            continue;
        }

        char quote = tag[start];
        size_t valueEnd = std::string::npos;
        std::string value;
        if (quote == '"' || quote == '\'') {
            ++start;
            valueEnd = tag.find(quote, start);
            if (valueEnd == std::string::npos) {
                pos = endTag + 1;
                continue;
            }
            value = tag.substr(start, valueEnd - start);
        } else {
            valueEnd = tag.find_first_of(" \t\r\n>", start);
            value = tag.substr(start, valueEnd - start);
        }

        const std::string resolved = resolveUrl(baseUrl, trim(value));
        if (!resolved.empty()) {
            links.push_back(resolved);
        }

        pos = endTag + 1;
    }

    return links;
}

std::string httpGet(const std::string& url) {
    HINTERNET hInternet = InternetOpenA("SimpleCrawler/1.0", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
    if (!hInternet) {
        throw std::runtime_error("InternetOpen failed");
    }

    HINTERNET hFile = InternetOpenUrlA(hInternet, url.c_str(), NULL, 0,
        INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_NO_COOKIES, 0);
    if (!hFile) {
        InternetCloseHandle(hInternet);
        throw std::runtime_error("InternetOpenUrl failed");
    }

    std::string response;
    char buffer[8192];
    DWORD bytesRead = 0;
    while (InternetReadFile(hFile, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        response.append(buffer, bytesRead);
    }

    InternetCloseHandle(hFile);
    InternetCloseHandle(hInternet);
    return response;
}

void crawl(const std::string& startUrl, int maxDepth, std::set<std::string>& visited, int depth = 0) {
    if (depth > maxDepth) {
        return;
    }

    if (!visited.insert(startUrl).second) {
        return;
    }

    std::cout << "[" << depth << "] GET " << startUrl << std::endl;

    try {
        const std::string html = httpGet(startUrl);
        std::cout << "\n===== Summary =====\n" << makeSummary(html, startUrl) << "\n===================\n";

        const std::vector<std::string> links = extractLinks(html, startUrl);

        for (const std::string& link : links) {
            const UrlParts baseParts = parseUrl(startUrl);
            const UrlParts linkParts = parseUrl(link);
            if (linkParts.host == baseParts.host) {
                crawl(link, maxDepth, visited, depth + 1);
            }
        }
    } catch (const std::exception& e) {
        std::cout << "  - Error: " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[]) {
    configureConsole();

    if (argc < 2) {
        std::cout << "Usage: webcrawler.exe http://example.com [depth]\n";
        return 1;
    }

    std::string url = normalizeUrl(argv[1]);
    int depth = 1;
    if (argc >= 3) {
        depth = std::atoi(argv[2]);
    }

    std::set<std::string> visited;
    crawl(url, depth, visited, 0);
    return 0;
}
