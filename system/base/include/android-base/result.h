#pragma once
//
// 精简树补丁：<android-base/result.h> 最小等价实现（libinit / libvold 重度使用）。
// 全 inline，header-only，调用方与实现方共用同一份头文件，布局自洽。
//

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <utility>

namespace android {
namespace base {

[[noreturn]] inline void ResultError(const char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  char buf[512];
  vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  fprintf(stderr, "ResultError: %s\n", buf);
  abort();
}

template <typename T = void>
class Result {
 public:
  static Result Ok() {
    Result r;
    r.ok_ = true;
    return r;
  }

  template <typename... Args>
  static Result Errorf(const char* fmt, Args... args) {
    return Result(std::string(fmt), std::string());
  }

  bool is_ok() const { return ok_; }
  explicit operator bool() const { return ok_; }
  const std::string& error() const { return error_msg_; }
  std::string& error() { return error_msg_; }

  T& operator*() { return value_; }
  const T& operator*() const { return value_; }
  T& value() { return value_; }
  const T& value() const { return value_; }

 private:
  Result(std::string error_msg, std::string) : ok_(false), error_msg_(std::move(error_msg)), value_() {}
  bool ok_ = false;
  std::string error_msg_;
  T value_{};
};

template <>
class Result<void> {
 public:
  static Result Ok() {
    Result r;
    r.ok_ = true;
    return r;
  }

  template <typename... Args>
  static Result Errorf(const char* fmt, Args... args) {
    return Result(std::string(fmt));
  }

  bool is_ok() const { return ok_; }
  explicit operator bool() const { return ok_; }
  const std::string& error() const { return error_msg_; }
  std::string& error() { return error_msg_; }

 private:
  explicit Result(std::string error_msg) : ok_(false), error_msg_(std::move(error_msg)) {}
  bool ok_ = false;
  std::string error_msg_;
};

}  // namespace base
}  // namespace android
