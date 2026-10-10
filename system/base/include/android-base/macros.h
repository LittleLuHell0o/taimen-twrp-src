#pragma once
//
// 精简树补丁：<android-base/macros.h> 最小等价实现。
// DISALLOW_COPY_AND_ASSIGN 被 system/core/fs_mgr/libsnapshot 之类的现代 C++ 代码
// 当类型属性用（class X { DISALLOW_COPY_AND_ASSIGN(X); };），必须给出。
//

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&) = delete;       \
  void operator=(const TypeName&) = delete

#define DISALLOW_COPY_AND_ASSIGN_OR_DEFAULT(TypeName, ...) \
  DISALLOW_COPY_AND_ASSIGN(TypeName)
#pragma once
//
// 精简树补丁：<android-base/macros.h> 最小等价实现。
//

#define DISALLOW_IMPLICIT_CONSTRUCTORS(ClassName) \
  ClassName();                                    \
  ClassName(const ClassName&);                    \
  void operator=(const ClassName&)

#define TEMPORARY_DESTRUCTOR(ClassName) ~ClassName()

#define ALWAYS_INLINE inline __attribute__((always_inline))
#define NEVER_INLINE __attribute__((noinline))
#define UNUSED __attribute__((unused))
#define NO_RETURN __attribute__((noreturn))
#define PRINTF_FORMAT(ar, first) __attribute__((format(printf, ar, first)))
