#pragma once
//
// 精简树补丁：system/base 被裁，这里是 <android-base/logging.h> 的最小等价实现。
//
// LogSeverity 的数值刻意对齐上游（VERBOSE=0 … FATAL=5）：
// bootable/recovery/recovery_main.cpp 的 UiLogger 直接拿 severity 当下标去索引
// log_characters[] = "VDIWEF"，靠的就是这套编号。
//

#include <functional>
#include <ostream>
#include <sstream>
#include <string>
#include <utility>
#include <cerrno>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>

namespace android {
namespace base {

enum LogId {
  LOG_ID_MAIN = 0,
  LOG_ID_SYSTEM = 1,
  LOG_ID_LOGD = 3,
  MAIN = 0,
  SYSTEM = 1,
  LOG_ID_DEFAULT = LOG_ID_MAIN,
};

enum LogSeverity {
  VERBOSE = 0,
  DEBUG = 1,
  INFO = 2,
  WARNING = 3,
  ERROR = 4,
  FATAL = 5,
  DEFAULT = INFO,
  NUM_LOG_SEVERITIES = 6,
};

using Logger = std::function<void(LogId, LogSeverity, const char* /*tag*/,
                                  const char* /*file*/, unsigned int /*line*/,
                                  const char* /*message*/)>;
using Aborter = std::function<void(const char*)>;

inline void DefaultAborter(const char* msg) {
  fprintf(stderr, "%s\n", msg);
  abort();
}

inline void SetLogger(Logger logger) {
  static Logger* logger_slot = []() -> Logger* {
    static Logger slot;
    return &slot;
  }();
  *logger_slot = std::move(logger);
}

inline Logger* GetLogger() {
  static Logger* logger_slot = []() -> Logger* {
    static Logger slot;
    return &slot;
  }();
  return logger_slot;
}

inline void SetAborter(Aborter aborter) {
  static Aborter* aborter_slot = []() -> Aborter* {
    static Aborter slot;
    return &slot;
  }();
  *aborter_slot = std::move(aborter);
}

inline Aborter* GetAborter() {
  static Aborter* aborter_slot = []() -> Aborter* {
    static Aborter slot;
    return &slot;
  }();
  return aborter_slot;
}

inline void SetMinimumLogSeverity(LogSeverity severity) {}
inline LogSeverity GetMinimumLogSeverity() { return DEFAULT; }

// 供 CHECK_* 使用的失败出口：返回 LogMessage&，这样 `CHECK(x);` 和
// `CHECK(x) << "msg";` 两种写法都能编译。
// 上游 logging.h 也是先把 LogMessage 前向声明，再给出 CheckFailed 的返回类型，
// 少了这一行就会 "unknown type name 'LogMessage'"。
class LogMessage;

LogMessage& CheckFailed(const char* file, int line, bool condition, const char* text);
LogMessage& CheckFailedStr(const char* file, int line, bool condition, const char* text);

class LogMessage {
 public:
  LogMessage(const char* file, int line, LogSeverity severity)
      : tag_(), file_(file), line_(line), severity_(severity), error_(0) {}
  LogMessage(const char* file, int line, LogSeverity severity, const char* tag)
      : tag_(tag), file_(file), line_(line), severity_(severity), error_(0) {}
  LogMessage(const char* file, int line, LogSeverity severity, int error, const char* func)
      : tag_(func ? func : ""), file_(file), line_(line), severity_(severity), error_(error) {}

  ~LogMessage() {
    std::string msg = stream_.str();
    if (!tag_.empty()) {
      msg = "[" + tag_ + "] " + msg;
    }
    if (error_ != 0) {
      msg += ": ";
      msg += std::strerror(error_);
    }
    std::string prefix =
        std::string(file_) + ":" + std::to_string(line_) + " ";
    msg = prefix + msg;
    Logger* logger = GetLogger();
    if (logger != nullptr && (*logger)) {
      (*logger)(LOG_ID_MAIN, severity_, tag_.empty() ? nullptr : tag_.c_str(),
                file_, static_cast<unsigned int>(line_), msg.c_str());
    } else {
      fprintf(stderr, "%s\n", msg.c_str());
    }
    if (severity_ == FATAL) {
      Aborter* aborter = GetAborter();
      if (aborter != nullptr && (*aborter)) {
        (*aborter)(msg.c_str());
      }
      DefaultAborter(msg.c_str());
    }
  }

  std::ostream& stream() { return stream_; }

  // AOSP 里 #define LOG(severity) LogMessage(...)，调用点会写
  // LOG(INFO)("fmt", ...)，所以 LogMessage 自己必须能当可调用对象。
  template <typename T>
  LogMessage& operator()(const T& value) {
    stream_ << value;
    return *this;
  }
  std::string& buffer() { return stream_str_; }

  // CHECK(x) << "msg"; 依赖这个流插入运算符（上游同样有），
  // 没有它 "*log << \"Check failed: \"" 会报 "invalid operands to binary expression"。
  template <typename T>
  LogMessage& operator<<(const T& value) {
    stream_ << value;
    return *this;
  }

 private:
  std::string tag_;
  // 必须是 const char*：Logger 的函数型参数是 const char*，
  // 存成 std::string 会让 (*logger)(..., file_, ...) 匹配不上。
  const char* file_;
  int line_;
  LogSeverity severity_;
  int error_;
  std::ostringstream stream_;
  std::string stream_str_;
};

inline void InitLogging(char** argv, Logger logger, Aborter aborter = nullptr) {
  SetLogger(std::move(logger));
  if (aborter) SetAborter(std::move(aborter));
}

inline void InitLogging(char** argv) {
  SetLogger(nullptr);
}

inline void KernelLogger(LogId id, LogSeverity severity, const char* tag, const char* file,
                         unsigned int line, const char* message) {
  fprintf(stderr, "%s\n", message);
}

inline void StderrLogger(LogId, LogSeverity, const char* tag, const char*, unsigned int,
                         const char* message) {
  fprintf(stderr, "%s\n", message);
}

inline void StdioLogger(LogId, LogSeverity, const char* tag, const char*, unsigned int,
                        const char* message) {
  fprintf(stdout, "%s\n", message);
}

class LogdLogger {
 public:
  explicit LogdLogger(LogId id) { (void)id; }
  void operator()(LogId id, LogSeverity severity, const char* tag, const char* file,
                  unsigned int line, const char* message) const {
    (void)id;
    fprintf(stderr, "%s\n", message);
  }

};

// 失败时 new 一个 LogMessage 并把引用交出去：流插入结束后（本语句末尾）
// 它的析构函数才把消息吐出来，符合 CHECK(...) << "msg"; 的用法。
// 条件为真时返回一个永不析构的哑对象，避免 static 析构在退出时乱打日志。
inline LogMessage& CheckFailed(const char* file, int line, bool condition, const char* text) {
  static LogMessage* never_destroyed = new LogMessage(file, line, INFO);
  if (!condition) {
    LogMessage* log = new LogMessage(file, line, FATAL);
    *log << "Check failed: " << text << " ";
    return *log;
  }
  return *never_destroyed;
}

inline LogMessage& CheckFailedStr(const char* file, int line, bool condition, const char* text) {
  return CheckFailed(file, line, condition, text);
}

}  // namespace base
}  // namespace android

#define LOG(severity) \
  android::base::LogMessage(__FILE__, __LINE__, android::base::severity)
#define PLOG(severity) \
  android::base::LogMessage(__FILE__, __LINE__, android::base::severity, errno, __FUNCTION__).stream()
#define PLOGINFO PLOG(INFO)
#define PLOGWARN PLOG(WARNING)
#define PLOGERROR PLOG(ERROR)
#define PLOGE PLOG(ERROR)
#define PLOGW PLOG(WARNING)
#define LOG_FATAL LOG(FATAL)

#define CHECK(condition) ::android::base::CheckFailed(__FILE__, __LINE__, (condition), #condition)
#define CHECK_EQ(a, b) ::android::base::CheckFailed(__FILE__, __LINE__, (a) == (b), #a " == " #b)
#define CHECK_NE(a, b) ::android::base::CheckFailed(__FILE__, __LINE__, (a) != (b), #a " != " #b)
#define CHECK_LT(a, b) ::android::base::CheckFailed(__FILE__, __LINE__, (a) < (b), #a " < " #b)
#define CHECK_LE(a, b) ::android::base::CheckFailed(__FILE__, __LINE__, (a) <= (b), #a " <= " #b)
#define CHECK_GT(a, b) ::android::base::CheckFailed(__FILE__, __LINE__, (a) > (b), #a " > " #b)
#define CHECK_GE(a, b) ::android::base::CheckFailed(__FILE__, __LINE__, (a) >= (b), #a " >= " #b)
#define CHECK_STR_EQ(a, b) \
  ::android::base::CheckFailedStr(__FILE__, __LINE__, (a) == (b), #a " == " #b)
#define CHECK_STR_NE(a, b) \
  ::android::base::CheckFailedStr(__FILE__, __LINE__, (a) != (b), #a " != " #b)
#define CHECK_NOTNULL(x) ::android::base::CheckFailed(__FILE__, __LINE__, (x) != nullptr, #x)
#define CHECK_FALSE(x) ::android::base::CheckFailed(__FILE__, __LINE__, !(x), "!(" #x ")")
#define CHECK_TRUE(x) ::android::base::CheckFailed(__FILE__, __LINE__, (x), #x)
