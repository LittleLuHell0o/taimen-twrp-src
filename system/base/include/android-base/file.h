#pragma once
//
// 精简树补丁：<android-base/file.h> 最小等价实现（system/base 被裁）。
// 全部 inline；实现只用到 bionic/libc 的 posix 接口。
//

#include <android-base/unique_fd.h>

#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>

#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <utility>
#include <vector>

namespace android {
namespace base {

inline bool ReadFully(int fd, void* data, size_t size) {
  uint8_t* buffer = reinterpret_cast<uint8_t*>(data);
  size_t done = 0;
  while (done < size) {
    ssize_t n = read(fd, buffer + done, size - done);
    if (n < 0) {
      if (errno == EINTR) continue;
      return false;
    }
    if (n == 0) return false;
    done += static_cast<size_t>(n);
  }
  return true;
}

inline bool ReadFullyAtOffset(int fd, void* data, size_t size, off64_t offset) {
  if (lseek64(fd, offset, SEEK_SET) == static_cast<off64_t>(-1)) return false;
  return ReadFully(fd, data, size);
}

inline bool WriteFully(int fd, const void* data, size_t size) {
  const uint8_t* buffer = reinterpret_cast<const uint8_t*>(data);
  size_t done = 0;
  while (done < size) {
    ssize_t n = write(fd, buffer + done, size - done);
    if (n < 0) {
      if (errno == EINTR) continue;
      return false;
    }
    done += static_cast<size_t>(n);
  }
  return true;
}

inline bool ReadFdToString(int fd, std::string* out, bool blocking = true) {
  std::string data;
  char buf[4096];
  for (;;) {
    ssize_t n = read(fd, buf, sizeof(buf));
    if (n < 0) {
      if (errno == EINTR) continue;
      return false;
    }
    if (n == 0) break;
    data.append(buf, static_cast<size_t>(n));
    if (!blocking) break;
  }
  if (out == nullptr) return false;
  *out = std::move(data);
  return true;
}

// 必须放在 ReadFdToString 之后：它体内调用了后者。
inline bool ReadFileToString(const std::string& path, std::string* out, bool follow_symlinks = true) {
  int fd = open(path.c_str(), O_RDONLY | (follow_symlinks ? 0 : O_NOFOLLOW));
  if (fd < 0) return false;
  bool ok = ReadFdToString(fd, out, true);
  close(fd);
  return ok;
}

inline bool WriteStringToFd(const std::string& data, int fd) {
  return WriteFully(fd, data.data(), data.size());
}

inline bool WriteStringToFile(const std::string& data, const std::string& path,
                              bool follow_symlinks = true) {
  int fd = open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, 0666);
  if (fd < 0) return false;
  if (!WriteStringToFd(data, fd)) {
    close(fd);
    return false;
  }
  return close(fd) == 0;
}

inline bool Readlink(const std::string& path, std::string* result) {
  result->resize(255);
  ssize_t len = readlink(path.c_str(), &(*result)[0], result->size());
  if (len < 0) return false;
  result->resize(static_cast<size_t>(len));
  return true;
}

inline bool Realpath(const std::string& path, std::string* result) {
  char buf[PATH_MAX];
  if (::realpath(path.c_str(), buf) == nullptr) return false;
  *result = buf;
  return true;
}

inline bool RemoveFileIfExists(const std::string& path) {
  struct stat st;
  if (lstat(path.c_str(), &st) == -1) {
    // Doesn't exist, so success.
    if (errno == ENOENT) return true;
    return false;
  }
  if (S_ISDIR(st.st_mode)) return rmdir(path.c_str()) == 0;
  return unlink(path.c_str()) == 0;
}

inline DIR* Fdopendir(int fd) { return fdopendir(fd); }

inline FILE* Fdopen(int fd, const char* mode) { return fdopen(fd, mode); }

inline std::string GetExecutablePath() {
  char buf[PATH_MAX];
  ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
  if (len < 0) return std::string();
  buf[len] = '\0';
  return std::string(buf);
}

inline std::string GetExecutableDirectory() {
  std::string path = GetExecutablePath();
  auto slash = path.rfind('/');
  if (slash == std::string::npos) return std::string();
  return path.substr(0, slash);
}

inline bool Pipe(int* fds) {
  return pipe(fds) == 0;
}

inline bool Socketpair(int domain, int type, int protocol, int sv[2]) {
  return ::socketpair(domain, type, protocol, sv) == 0;
}

}  // namespace base
}  // namespace android
