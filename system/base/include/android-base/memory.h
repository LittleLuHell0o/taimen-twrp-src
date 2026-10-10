#pragma once
//
// 精简树补丁：<android-base/memory.h> 最小等价实现。
//

#include <memory>
#include <utility>

namespace android {
namespace base {

template <typename T, typename... Args>
std::unique_ptr<T> MakeUnique(Args&&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

}  // namespace base
}  // namespace android
