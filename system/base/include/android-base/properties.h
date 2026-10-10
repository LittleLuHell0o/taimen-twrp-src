#pragma once
//
// 精简源码树补丁：system/base 整包被裁，而 bootable/recovery（以及 libinit /
// libvold）都要 #include <android-base/properties.h>。
//
// 这里给一份等价的最小实现。刻意「全部 inline」，不去链接 prebuilt 的
// libbase.so：一是精简树里 libbase 只有 .so 没有源码，无法确认接口签名，
// inline 实现可以绕开 ABI（返回值传递位置）对齐风险；二是只依赖 bionic 的
// __system_property_get / __system_property_set。
//
// 注意签名对齐两点：
//   * GetIntProperty<T> 是模板（prebuilt libbase.so 里对应符号是 W 弱符号），
//     参数形如 (const std::string&, T, T, T)；
//   * WaitForProperty 在 TWRP 里是当 bool 用的
//     （bootable/recovery/*/*.cpp 里 "return android::base::WaitForProperty(...)"），
//     所以返回值给 bool，而不是 AOSP 上游的 void。
//

#include <chrono>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <string>
#include <unistd.h>

#ifndef ANDROID_BASE_PROPERTIES_SHIM
#define ANDROID_BASE_PROPERTIES_SHIM

#ifndef PROP_VALUE_MAX
#define PROP_VALUE_MAX 92
#endif

extern "C" int __system_property_get(const char* name, char* value);
extern "C" int __system_property_set(const char* name, const char* value);

namespace android {
namespace base {

inline std::string GetProperty(const std::string& name, const std::string& default_value) {
  if (name.empty()) return default_value;
  char prop_val[PROP_VALUE_MAX] = {};
  if (__system_property_get(name.c_str(), prop_val) > 0 && prop_val[0] != '\0') {
    return std::string(prop_val);
  }
  return default_value;
}

inline void SetProperty(const std::string& name, const std::string& value) {
  if (name.empty()) return;
  __system_property_set(name.c_str(), value.c_str());
}

inline bool GetBoolProperty(const std::string& name, bool default_value) {
  const std::string prop_str = GetProperty(name, std::string());
  if (prop_str.empty()) return default_value;
  return (prop_str == "true" || prop_str == "True" || prop_str == "TRUE" ||
          prop_str == "yes" || prop_str == "1");
}

template <typename T>
inline T GetIntProperty(const std::string& name, T default_value,
                        T min_value = std::numeric_limits<T>::min(),
                        T max_value = std::numeric_limits<T>::max()) {
  const std::string prop_str = GetProperty(name, std::string());
  if (prop_str.empty()) return default_value;
  errno = 0;
  char* end = nullptr;
  const long long parsed = strtoll(prop_str.c_str(), &end, 0);
  if (end == prop_str.c_str() || errno == ERANGE) return default_value;
  T out = static_cast<T>(parsed);
  if (out < min_value) out = min_value;
  if (out > max_value) out = max_value;
  return out;
}

template <typename T>
inline T GetUintProperty(const std::string& name, T default_value,
                         T min_value = 0,
                         T max_value = std::numeric_limits<T>::max()) {
  return GetIntProperty<T>(name, default_value, min_value, max_value);
}

template <typename Rep, typename Period>
inline bool WaitForProperty(const std::string& name, const std::string& value,
                            std::chrono::duration<Rep, Period> timeout =
                                std::chrono::milliseconds(0)) {
  long long waited_ms = 0;
  const long long step_ms = 50;
  for (;;) {
    if (GetProperty(name, std::string()) == value) return true;
    const long long total_ms =
        static_cast<long long>(std::chrono::duration_cast<std::chrono::milliseconds>(
                                   std::chrono::duration<Rep, Period>(timeout))
                                   .count());
    if (waited_ms >= total_ms) return false;
    ::usleep(static_cast<unsigned int>(step_ms) * 1000);
    waited_ms += step_ms;
  }
}

}  // namespace base
}  // namespace android

#endif  // ANDROID_BASE_PROPERTIES_SHIM
