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
#include <stdlib.h>

typedef uint64_t BN_ULONG;
typedef struct bignum_st BN;
typedef struct bn_ctx_st BN_CTX;

#define BN_R_LOW_BITS(num) ((num) & (((BN_ULONG)1 - 1)))

BN *BN_new(void);
void BN_free(BN *a);
BN *BN_bin2bn(const unsigned char *s, int len, BN *ret);
BN *BN_le2bn(const unsigned char *s, int len, BN *ret);
int BN_bn2le_padded(unsigned char *to, int tolen, const BN *a);
int BN_num_bits(const BN *a);
unsigned long BN_get_word(const BN *a);
int BN_set_word(BN *a, unsigned long w);
int BN_set_bit(BN *a, int n);
int BN_sub(BN *r, BN *a, BN *b);
int BN_mod(BN *r, BN *a, BN *b, BN_CTX *ctx);
BN *BN_mod_inverse(BN *r, BN *a, BN *b, BN_CTX *ctx);
int BN_mod_sqr(BN *r, BN *a, BN *b, BN_CTX *ctx);

BN_CTX *BN_CTX_new(void);
void BN_CTX_free(BN_CTX *c);


#ifdef __cplusplus
}
#endif
