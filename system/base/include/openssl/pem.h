#pragma once
//
// 精简树补丁：<openssl/xxx.h> 的最小「声明式」等价物。
// 只有类型/函数签名，实现由 prebuilt libcrypto.so 提供。
// 刻意用 extern "C"，因为桩位全部是 C 链接的函数。
//

#ifdef __cplusplus
extern "C" {
#endif


struct x509_st;
typedef struct x509_st X509;
struct bio_st;
typedef struct bio_st BIO;
struct evp_pkey_st;
typedef struct evp_pkey_st EVP_PKEY;

X509 *PEM_read_bio_X509(BIO *bp, X509 **x, void * (*cb)(void *), void *u);
EVP_PKEY *PEM_read_bio_PrivateKey(BIO *bp, EVP_PKEY **x,
                                  void *(*cb)(void *), void *u);
EVP_PKEY *PEM_read_bio_PUBKEY(BIO *bp, EVP_PKEY **x, void *(*cb)(void *), void *u);
void X509_free(X509 *x);


#ifdef __cplusplus
}
#endif
