#pragma once
//
// 精简树补丁：<android-base/parseint.h> 最小等价实现。
//

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

#include <limits>
#include <string>
#include <string_view>

namespace android {
namespace base {

template <typename T>
inline bool ParseUnsignedIntWithBase(std::string_view s, T* out, int base) {
  if (s.empty()) return false;
  errno = 0;
  char* end = nullptr;
  unsigned long long parsed = strtoull(std::string(s).c_str(), &end, base);
  if (end != std::string(s).c_str() + s.size() || errno == ERANGE) return false;
  if (parsed > static_cast<unsigned long long>(std::numeric_limits<T>::max())) return false;
  *out = static_cast<T>(parsed);
  return true;
}

template <typename T>
inline bool ParseInt(std::string_view s, T* out, int64_t min_value = std::numeric_limits<T>::min(),
                     int64_t max_value = std::numeric_limits<T>::max()) {
  if (s.empty()) return false;
  bool negative = s.front() == '-';
  if (negative) s.remove_prefix(1);
  if (s.empty()) return false;
  T result = 0;
  if (!ParseUnsignedIntWithBase<T>(s, &result, 10)) return false;
  int64_t signed_value = negative ? -static_cast<int64_t>(result) : static_cast<int64_t>(result);
  if (signed_value < min_value || signed_value > max_value) return false;
  *out = static_cast<T>(signed_value);
  return true;
}

template <typename T>
inline bool ParseUint(std::string_view s, T* out, int64_t min_value = 0,
                      int64_t max_value = std::numeric_limits<T>::max()) {
  if (s.empty()) return false;
  T result = 0;
  if (!ParseUnsignedIntWithBase<T>(s, &result, 10)) return false;
  if (static_cast<int64_t>(result) < min_value || static_cast<int64_t>(result) > max_value) return false;
  *out = result;
  return true;
}

template <typename T>
inline bool ParseUnsignedInt(std::string_view s, T* out, int base = 10) {
  if (s.empty()) return false;
  return ParseUnsignedIntWithBase<T>(s, out, base);
}

inline bool ParseByteCount(std::string_view s, int64_t* out) {
  if (s.empty()) return false;
  if (s.back() == 'B' || s.back() == 'b') s.remove_suffix(1);
  int64_t value = 0;
  if (!ParseInt<int64_t>(s, &value)) return false;
  *out = value;
  return true;
}

}  // namespace base
}  // namespace android
