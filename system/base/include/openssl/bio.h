#pragma once
//
// 精简树补丁：<openssl/xxx.h> 的最小「声明式」等价物。
// 只有类型/函数签名，实现由 prebuilt libcrypto.so 提供。
// 刻意用 extern "C"，因为桩位全部是 C 链接的函数。
//

#ifdef __cplusplus
extern "C" {
#endif


typedef struct bio_st BIO;
typedef struct bio_method_st BIO_METHOD;

BIO *BIO_new_mem_buf(const void *buf, int len);
void BIO_free(BIO *a);
BIO_METHOD *BIO_s_mem(void);


#ifdef __cplusplus
}
#endif
