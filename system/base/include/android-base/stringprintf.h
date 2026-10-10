#pragma once
//
// 精简树补丁：<android-base/stringprintf.h> 最小等价实现。
//

#include <stdarg.h>

#include <string>
#include <utility>

namespace android {
namespace base {

inline std::string StringPrintf(const char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int result = vsnprintf(nullptr, 0, fmt, ap);
  va_end(ap);
  std::string out;
  if (result < 0) return out;
  out.resize(static_cast<size_t>(result));
  va_start(ap, fmt);
  vsnprintf(&out[0], out.size() + 1, fmt, ap);
  va_end(ap);
  return out;
}

inline std::string StringAppendV(std::string* output, const char* fmt, va_list ap) {
  int result = vsnprintf(nullptr, 0, fmt, ap);
  if (result < 0) return std::string();
  size_t offset = output->size();
  output->resize(offset + static_cast<size_t>(result));
  va_list ap2;
  va_copy(ap2, ap);
  vsnprintf(&(*output)[offset], static_cast<size_t>(result) + 1, fmt, ap2);
  va_end(ap2);
  return *output;
}

inline std::string StringAppendF(std::string* output, const char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  std::string result = StringAppendV(output, fmt, ap);
  va_end(ap);
  return result;
}

}  // namespace base
}  // namespace android
