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

struct evp_cipher_ctx_st;
typedef struct evp_cipher_ctx_st EVP_CIPHER_CTX;

struct evp_pkey_st;
typedef struct evp_pkey_st EVP_PKEY;

struct env_md_ctx_st;
typedef struct env_md_ctx_st EVP_MD_CTX;

typedef struct evp_cipher_st EVP_CIPHER;
typedef struct env_md_st EVP_MD;

#define EVP_PKEY_NONE 0
#define EVP_PKEY_RSA 6
#define EVP_PKEY_EC 3
#define EVP_PKEY_ED25519 47
#define EVP_PKEY_X25519 48

#define EVP_CTRL_GCM_SET_TAG 0x9
#define EVP_CTRL_GCM_GET_TAG 0x10
#define EVP_CTRL_AEAD_SET_TAG EVP_CTRL_GCM_SET_TAG
#define EVP_CTRL_AEAD_GET_TAG EVP_CTRL_GCM_GET_TAG

#define EVP_MAX_MD_SIZE 64
#define EVP_MAX_BLOCK_LENGTH 32

/* --- 对称密码 --- */
EVP_CIPHER_CTX *EVP_CIPHER_CTX_new(void);
void EVP_CIPHER_CTX_free(EVP_CIPHER_CTX *ctx);
void EVP_CIPHER_CTX_init(EVP_CIPHER_CTX *ctx);
void EVP_CIPHER_CTX_cleanup(EVP_CIPHER_CTX *c);
int EVP_CIPHER_CTX_ctrl(EVP_CIPHER_CTX *ctx, int type, int arg, void *ptr);
int EVP_CIPHER_CTX_set_padding(EVP_CIPHER_CTX *ctx, int padding);
int EVP_EncryptInit(EVP_CIPHER_CTX *ctx, const EVP_CIPHER *cipher,
                    const unsigned char *key, const unsigned char *iv);
int EVP_EncryptInit_ex(EVP_CIPHER_CTX *ctx, const EVP_CIPHER *cipher,
                       void *engine, const unsigned char *key,
                       const unsigned char *iv);
int EVP_EncryptUpdate(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl,
                      const unsigned char *in, int inl);
int EVP_EncryptFinal(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl);
int EVP_EncryptFinal_ex(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl);
int EVP_DecryptInit(EVP_CIPHER_CTX *ctx, const EVP_CIPHER *cipher,
                    const unsigned char *key, const unsigned char *iv);
int EVP_DecryptInit_ex(EVP_CIPHER_CTX *ctx, const EVP_CIPHER *cipher,
                       void *engine, const unsigned char *key,
                       const unsigned char *iv);
int EVP_DecryptUpdate(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl,
                      const unsigned char *in, int inl);
int EVP_DecryptFinal(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl);
int EVP_DecryptFinal_ex(EVP_CIPHER_CTX *ctx, unsigned char *out, int *outl);

const EVP_CIPHER *EVP_aes_128_cbc(void);
const EVP_CIPHER *EVP_aes_128_ecb(void);
const EVP_CIPHER *EVP_aes_256_gcm(void);
const EVP_CIPHER *EVP_aes_256_cbc(void);
const EVP_MD *EVP_sha256(void);
const EVP_MD *EVP_sha1(void);

/* --- PKEY --- */
int EVP_PKEY_id(const EVP_PKEY *pkey);
EVP_PKEY *EVP_PKEY_new(void);
void EVP_PKEY_free(EVP_PKEY *pkey);
EVP_PKEY *EVP_PKEY_get1_RSA(EVP_PKEY *pkey);
EVP_PKEY *EVP_PKEY_get1_EC_KEY(EVP_PKEY *pkey);

/* --- base64 --- */
size_t EVP_DecodedLength(size_t *out_len, size_t len);
int EVP_DecodeBase64(unsigned char *out, size_t *out_len, size_t max_out,
                     const unsigned char *in, size_t in_len);
int EVP_EncodeBlock(unsigned char *t, const unsigned char *f, int n);

/* --- md ctx（TWRP 的 twrpdigest 走这条路）--- */
EVP_MD_CTX *EVP_MD_CTX_new(void);
EVP_MD_CTX *EVP_MD_CTX_create(void);
void EVP_MD_CTX_free(EVP_MD_CTX *ctx);
void EVP_MD_CTX_destroy(EVP_MD_CTX *ctx);
int EVP_DigestInit_ex(EVP_MD_CTX *ctx, const EVP_MD *type, void *engine);
int EVP_DigestInit(EVP_MD_CTX *ctx, const EVP_MD *type);
int EVP_DigestUpdate(EVP_MD_CTX *ctx, const void *d, size_t cnt);
int EVP_DigestFinal(EVP_MD_CTX *ctx, unsigned char *md);
int EVP_DigestFinal_ex(EVP_MD_CTX *ctx, unsigned char *md);


#ifdef __cplusplus
}
#endif
