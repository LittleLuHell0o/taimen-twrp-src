#pragma once
//
// 精简树补丁：让 libc++ 的 <cmath> 能在「<math.h> 被解析到 bionic 原版」时编过。
//
// 背景
// ----
// bionic/libc/include/math.h 把这些分类/关系判定名字只做成【函数式宏】：
//     #define signbit(x)      ...                             (math.h:71)
//     #define fpclassify(x)   ...                             (math.h:61)
//     #define isfinite(x)     ...                             (math.h:63)
//     #define isnormal(x)     ...                             (math.h:69)
//     #define isgreater(x, y) __builtin_isgreater((x), (y))   (math.h:305)
//     ...（isgreaterequal / isless / islessequal / islessgreater / isunordered 同）
//
// 而 libc++ (LLVM 12) 的 <cmath>:321-333 写的是
//     using ::signbit;
//     using ::isgreater;
// 函数式宏在 token 后面没跟 '(' 时不会展开，于是这些 token 变成未声明的名字：
//     error: no member named 'isgreater' in the global namespace
//
// 为什么 libc++ 自己没兜住：libc++ 的 include/math.h 里本来有「把宏换成模板函数」
// 的块（#ifdef signbit ... #undef signbit ...），但那要求 `#include <math.h>`
// 命中 libc++ 的包装头。本树实测 <math.h> 直接被解析到 bionic/libc/include/math.h
// （clang 把所有 -I 目录排在所有 -isystem 目录之前），包装头根本没参与编译，
// 所以宏一直是宏。此结论由「真实编译命令 + 增删 -include 的 A/B 对照」确认。
//
// 做法
// ----
// 本头 #include <math.h> 拿到宏后，逐个 #ifdef 判断：
//   * 是宏  → #undef 并补一个语义完全一致（__builtin_* 薄封装）的全局内联函数；
//   * 已是真函数声明 → 不碰（bionic 对 isinf/isnan 用 (name) 技巧给过声明）。
// 这样 `using ::X;` 与 `X(a,b)` 两种用法都能工作。
//
// 由 build/soong/cc/config/global.go 的 "-include <abs path>" 强制前置。
//
#ifndef __CUSTOM_REL_MATH_FIX_H__
#define __CUSTOM_REL_MATH_FIX_H__

#ifdef __cplusplus

#include <math.h>

#ifdef signbit
#undef signbit
inline bool signbit(double __x) { return __builtin_signbit(__x); }
#endif

#ifdef fpclassify
#undef fpclassify
inline int fpclassify(double __x) {
  return __builtin_fpclassify(FP_NAN, FP_INFINITE, FP_NORMAL, FP_SUBNORMAL, FP_ZERO, __x);
}
#endif

#ifdef isfinite
#undef isfinite
inline bool isfinite(double __x) { return __builtin_isfinite(__x); }
#endif

#ifdef isnormal
#undef isnormal
inline bool isnormal(double __x) { return __builtin_isnormal(__x); }
#endif

#ifdef isgreater
#undef isgreater
inline bool isgreater(double __x, double __y) { return __builtin_isgreater(__x, __y); }
#endif

#ifdef isgreaterequal
#undef isgreaterequal
inline bool isgreaterequal(double __x, double __y) { return __builtin_isgreaterequal(__x, __y); }
#endif

#ifdef isless
#undef isless
inline bool isless(double __x, double __y) { return __builtin_isless(__x, __y); }
#endif

#ifdef islessequal
#undef islessequal
inline bool islessequal(double __x, double __y) { return __builtin_islessequal(__x, __y); }
#endif

#ifdef islessgreater
#undef islessgreater
inline bool islessgreater(double __x, double __y) { return __builtin_islessgreater(__x, __y); }
#endif

#ifdef isunordered
#undef isunordered
inline bool isunordered(double __x, double __y) { return __builtin_isunordered(__x, __y); }
#endif

#endif  /* __cplusplus */
#endif  /* __CUSTOM_REL_MATH_FIX_H__ */
