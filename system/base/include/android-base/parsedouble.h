#pragma once
//
// 精简树补丁：<android-base/parsedouble.h> 最小等价实现。
//

#include <errno.h>
#include <stdlib.h>

#include <string>
#include <string_view>

namespace android {
namespace base {

inline bool ParseDouble(std::string_view s, double* out) {
  if (s.empty()) return false;
  const std::string str(s);
  errno = 0;
  char* end = nullptr;
  double result = strtod(str.c_str(), &end);
  if (end != str.c_str() + str.size() || errno == ERANGE) return false;
  *out = result;
  return true;
}

}  // namespace base
}  // namespace android
