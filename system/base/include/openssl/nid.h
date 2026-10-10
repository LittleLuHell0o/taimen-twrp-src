#pragma once
//
// 精简树补丁：<openssl/xxx.h> 的最小「声明式」等价物。
// 只有类型/函数签名，实现由 prebuilt libcrypto.so 提供。
// 刻意用 extern "C"，因为桩位全部是 C 链接的函数。
//

#ifdef __cplusplus
extern "C" {
#endif


/* 只需要被用到的 NID 常量；数值沿用 OpenSSL/BoringSSL 的公开定义。 */
#define NID_undef 0
#define NID_sha1 58
#define NID_sha256 672
#define NID_sha512 674
#define NID_rsaEncryption 6
#define NID_X9_62_prime256v1 415
#define NID_secp384r1 715
#define NID_secp521r1 716
#define NID_X25519 1025
#define NID_ed25519 1035


#ifdef __cplusplus
}
#endif
