#pragma once
//
// 精简树补丁：<android-base/strings.h> 最小等价实现（system/base 被裁）。
// 全部 inline，符号形态对齐 prebuilt libbase.so 的 llvm-nm 输出
// （StartsWith(string_view, string_view) / StartsWith(string_view, char)、
//  StringReplace(string_view, string_view, string_view, bool)）。
//

#include <strings.h>  // strcasecmp
#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

#include <string_view>

namespace android {
namespace base {

inline std::string Trim(std::string_view s) {
  auto begin = s.begin();
  while (begin != s.end() && std::isspace(static_cast<unsigned char>(*begin))) ++begin;
  auto end = s.end();
  while (end != begin && std::isspace(static_cast<unsigned char>(*(end - 1)))) --end;
  return std::string(begin, end);
}

inline bool StartsWith(std::string_view s, std::string_view prefix) {
  return s.size() >= prefix.size() && s.compare(0, prefix.size(), prefix) == 0;
}
inline bool StartsWith(std::string_view s, char prefix) {
  return !s.empty() && s.front() == prefix;
}
inline bool EndsWith(std::string_view s, std::string_view suffix) {
  return s.size() >= suffix.size() &&
         s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
}
inline bool EndsWith(std::string_view s, char suffix) {
  return !s.empty() && s.back() == suffix;
}
inline bool EndsWithIgnoreCase(std::string_view s, std::string_view suffix) {
  if (s.size() < suffix.size()) return false;
  auto begin = s.begin();
  std::advance(begin, -static_cast<ptrdiff_t>(suffix.size()));
  return std::equal(suffix.begin(), suffix.end(), begin, [](char a, char b) {
    return std::tolower(static_cast<unsigned char>(a)) == std::tolower(static_cast<unsigned char>(b));
  });
}
inline bool EqualsIgnoreCase(std::string_view a, std::string_view b) {
  if (a.size() != b.size()) return false;
  return std::equal(a.begin(), a.end(), b.begin(), [](char x, char y) {
    return std::tolower(static_cast<unsigned char>(x)) == std::tolower(static_cast<unsigned char>(y));
  });
}
inline bool ConsumePrefix(std::string_view* s, std::string_view prefix) {
  if (!StartsWith(*s, prefix)) return false;
  s->remove_prefix(prefix.size());
  return true;
}
inline std::string Basename(std::string_view path) {
  auto pos = path.rfind('/');
  if (pos == std::string_view::npos) return std::string(path);
  if (pos == path.size() - 1) return std::string();
  return std::string(path.substr(pos + 1));
}
inline std::string Dirname(std::string_view path) {
  auto pos = path.rfind('/');
  if (pos == std::string_view::npos) return std::string(".");
  if (pos == 0) return std::string("/");
  return std::string(path.substr(0, pos));
}
inline std::string Join(const std::vector<std::string>& s, char c) {
  std::string result;
  for (size_t i = 0; i < s.size(); ++i) {
    if (i != 0) result.push_back(c);
    result += s[i];
  }
  return result;
}
inline std::string Join(const std::vector<std::string>& s, std::string_view c) {
  std::string result;
  for (size_t i = 0; i < s.size(); ++i) {
    if (i != 0) result += c;
    result += s[i];
  }
  return result;
}
inline std::vector<std::string> Split(std::string_view s, char c) {
  std::vector<std::string> result;
  size_t last = 0;
  for (size_t i = 0; i <= s.size(); ++i) {
    if (i == s.size() || s[i] == c) {
      result.emplace_back(s.substr(last, i - last));
      last = i + 1;
    }
  }
  return result;
}
inline std::string StringReplace(std::string_view s, std::string_view from, std::string_view to,
                                 bool all = true) {
  if (from.empty()) return std::string(s);
  std::string result;
  size_t pos = 0;
  for (;;) {
    size_t found = s.find(from, pos);
    if (found == std::string_view::npos) break;
    result.append(s.substr(pos, found - pos));
    result.append(to);
    pos = found + from.size();
    if (!all) break;
  }
  result.append(s.substr(pos));
  return result;
}

}  // namespace base
}  // namespace android
