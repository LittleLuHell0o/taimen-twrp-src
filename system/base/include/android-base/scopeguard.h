#pragma once
//
// 精简树补丁：<android-base/scopeguard.h> 最小等价实现。
//

#include <utility>

namespace android {
namespace base {

template <typename F>
class ScopeGuard {
 public:
  static ScopeGuard Make(F&& f) { return ScopeGuard(std::forward<F>(f)); }
  explicit ScopeGuard(F&& f) : f_(std::forward<F>(f)), dismissed_(false) {}
  ScopeGuard(ScopeGuard&& other) : f_(std::move(other.f_)), dismissed_(other.dismissed_) {
    other.dismissed_ = true;
  }
  ScopeGuard(const ScopeGuard&) = delete;
  ScopeGuard& operator=(const ScopeGuard&) = delete;
  ~ScopeGuard() {
    if (!dismissed_) f_();
  }
  void dismiss() { dismissed_ = true; }

 private:
  F f_;
  bool dismissed_;
};

template <typename F>
ScopeGuard<F> make_scope_guard(F&& f) {
  return ScopeGuard<F>(std::forward<F>(f));
}

}  // namespace base
}  // namespace android
