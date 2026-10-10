#pragma once
//
// 精简树补丁：<openssl/xxx.h> 的最小「声明式」等价物。
// 只有类型/函数签名，实现由 prebuilt libcrypto.so 提供。
// 刻意用 extern "C"，因为桩位全部是 C 链接的函数。
//

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>

struct rsa_st;
typedef struct rsa_st RSA;
struct bignum_st;
typedef struct bignum_st BIGNUM;

#define RSA_KEY_SIZE 128
#define RSA_KEY_SIZE_BYTES 64
#define RSA_EXPONENT 65537

RSA *RSA_new(void);
void RSA_free(RSA *r);
int RSA_size(const RSA *r);
int RSA_generate_key_ex(RSA *rsa, int bits, BIGNUM *e, void *cb);
int RSA_set0_key(RSA *r, BIGNUM *n, BIGNUM *e, BIGNUM *d);
void RSA_get0_key(const RSA *r, const BIGNUM **n, const BIGNUM **e,
                  const BIGNUM **d);
int RSA_verify(int type, const unsigned char *dgst, size_t dgstlen,
               const unsigned char *sig, size_t siglen, RSA *rsa);


#ifdef __cplusplus
}
#endif
