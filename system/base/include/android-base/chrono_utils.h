#pragma once
//
// 精简树补丁：<android-base/chrono_utils.h> 最小等价实现。
//

#include <chrono>

namespace android {
namespace base {

using boot_clock = std::chrono::steady_clock;

class Timer {
 public:
  explicit Timer(bool autostart = true) : start_(autostart ? now() : boot_clock::time_point()) {}
  void Reset() { start_ = now(); }
  double Elapsed() const {
    return std::chrono::duration<double, std::milli>(now() - start_).count();
  }
  double ElapsedRealtime() const { return Elapsed(); }
  double ElapsedRealtimeWall() const { return Elapsed(); }

  // TWRP: while (t.duration() < std::chrono::nanoseconds(...)) ...
  std::chrono::nanoseconds duration() const {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(now() - start_);
  }

 private:
  static boot_clock::time_point now() { return boot_clock::now(); }
  boot_clock::time_point start_;
};

}  // namespace base
}  // namespace android
