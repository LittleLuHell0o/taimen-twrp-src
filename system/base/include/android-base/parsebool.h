#pragma once
//
// 精简树补丁：<android-base/parsebool.h> 最小等价实现。
//

#include <string_view>

namespace android {
namespace base {

inline bool ParseBool(std::string_view s, bool* out) {
  if (s == "0" || s == "false" || s == "no" || s == "off" || s == "F" || s == "f") {
    *out = false;
    return true;
  }
  if (s == "1" || s == "true" || s == "yes" || s == "on" || s == "t" || s == "T") {
    *out = true;
    return true;
  }
  return false;
}

inline bool ParseBool(std::string_view s, bool* out, bool* out_err) {
  if (!ParseBool(s, out)) {
    *out_err = true;
    return false;
  }
  *out_err = false;
  return true;
}

}  // namespace base
}  // namespace android
