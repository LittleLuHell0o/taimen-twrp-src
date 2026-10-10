#pragma once
//
// 精简树补丁：<android-base/unique_fd.h> 最小等价实现。
//

#include <unistd.h>

#include <type_traits>
#include <utility>

namespace android {
namespace base {

using borrowed_fd = int;

class unique_fd {
 public:
  unique_fd() : fd_(-1) {}
  explicit unique_fd(int fd) : fd_(fd) {}
  unique_fd(unique_fd&& other) : fd_(other.release()) {}
  unique_fd(std::nullptr_t) : fd_(-1) {}
  unique_fd& operator=(unique_fd&& other) {
    unique_fd(std::move(other)).swap(*this);
    return *this;
  }
  ~unique_fd() { reset(); }

  bool operator()() const { return fd_ >= 0; }
  explicit operator bool() const { return fd_ >= 0; }
  unique_fd& operator=(int fd) {
    reset(fd);
    return *this;
  }
  bool operator==(std::nullptr_t) const { return fd_ < 0; }
  bool operator!=(std::nullptr_t) const { return fd_ >= 0; }

  int get() const { return fd_; }
  int release() {
    int fd = fd_;
    fd_ = -1;
    return fd;
  }
  void reset(int fd = -1) {
    if (fd_ >= 0) close(fd_);
    fd_ = fd;
  }
  void swap(unique_fd& other) { std::swap(fd_, other.fd_); }

 private:
  int fd_;
};

// 精简树补丁（Round17）：真实 AOSP 的 unique_fd.h 还提供「fd 与裸 int 比较」的
// 非成员运算符，调用点大量写 `if (fd != -1)` / `fd == -1`（bootable/recovery、
// system/vold 里都有）。只给成员 operator!=(std::nullptr_t) 时，nullptr_t
// 不能隐式转成 int，于是 clang 报
//   error: invalid operands to binary expression
//        ('const android::base::unique_fd' and 'int')
// 并罗列 optional/map/regex 的一堆候选模板 note。
// 这里两个操作数顺序都补上，避免调用点写法不同再炸一次。
// 注意：nullptr_t 不会隐式转 int，所以与成员 operator==(std::nullptr_t)
// 不构成重载歧义。
inline bool operator==(const unique_fd& ufd, borrowed_fd fd) { return ufd.get() == fd; }
inline bool operator!=(const unique_fd& ufd, borrowed_fd fd) { return ufd.get() != fd; }
inline bool operator==(borrowed_fd fd, const unique_fd& ufd) { return fd == ufd.get(); }
inline bool operator!=(borrowed_fd fd, const unique_fd& ufd) { return fd != ufd.get(); }

}  // namespace base
}  // namespace android
