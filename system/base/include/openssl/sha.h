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

typedef struct SHA256state_st {
  uint32_t h[8];
  uint32_t Nl;
  uint32_t Nh;
  union {
    uint8_t p[64];
    uint32_t d[16];
  } u;
  unsigned int num;
} SHA256_CTX;

#define SHA256_CBLOCK 64
#define SHA256_DIGEST_LENGTH 32
#define SHA256_DIGEST_SIZE 32
#define SHA256_HEX_SIZE 65

void SHA256_Init(SHA256_CTX *c);
void SHA256_Update(SHA256_CTX *c, const void *data, size_t len);
void SHA256_Final(unsigned char *md, SHA256_CTX *c);
char *SHA256_End(SHA256_CTX *c, char *md);
char *SHA256_Sum(const unsigned char *d, size_t n, char *md);

void SHA1_Init(void *c);
void SHA1_Update(void *c, const void *data, size_t len);
void SHA1_Final(unsigned char *md, void *c);
void SHA512_Init(void *c);
void SHA512_Update(void *c, const void *data, size_t len);
void SHA512_Final(unsigned char *md, void *c);


#ifdef __cplusplus
}
#endif
