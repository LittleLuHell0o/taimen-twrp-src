#pragma once
//
// 精简树补丁：<android-base/thread_annotations.h> 空实现（无 clang-thread-safety 注解需求）。
//

#ifndef GUARDED_BY
#define GUARDED_BY(mtx)
#endif
#ifndef EXCLUSIVE_LOCKS_REQUIRED
#define EXCLUSIVE_LOCKS_REQUIRED(...)
#endif
#ifndef SHARED_LOCKS_REQUIRED
#define SHARED_LOCKS_REQUIRED(...)
#endif
#ifndef LOCKS_EXCLUDED
#define LOCKS_EXCLUDED(...)
#endif
#ifndef THREAD_SAFETY_ANALYSIS
#define THREAD_SAFETY_ANALYSIS
#endif
#ifndef CAPABILITY
#define CAPABILITY(x)
#endif
#ifndef ARGUMENT_LOCKED
#define ARGUMENT_LOCKED(...)
#endif
#ifndef RETURN_CAPABILITY
#define RETURN_CAPABILITY(x)
#endif
#ifndef NO_THREAD_SAFETY_ANALYSIS
#define NO_THREAD_SAFETY_ANALYSIS
#endif
