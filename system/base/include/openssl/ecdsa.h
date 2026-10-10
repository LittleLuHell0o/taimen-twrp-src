#pragma once
//
// 精简树补丁：<openssl/xxx.h> 的最小「声明式」等价物。
// 只有类型/函数签名，实现由 prebuilt libcrypto.so 提供。
// 刻意用 extern "C"，因为桩位全部是 C 链接的函数。
//

#ifdef __cplusplus
extern "C" {
#endif


#include <stddef.h>
#include <stdint.h>

struct ec_key_st;
typedef struct ec_key_st EC_KEY;

int ECDSA_verify(int type, const unsigned char *dgst, size_t dgstlen,
                 const unsigned char *sig, size_t siglen, EC_KEY *eckey);


#ifdef __cplusplus
}
#endif
