#pragma once
//
// 精简树补丁：<openssl/xxx.h> 的最小「声明式」等价物。
// 只有类型/函数签名，实现由 prebuilt libcrypto.so 提供。
// 刻意用 extern "C"，因为桩位全部是 C 链接的函数。
//

#ifdef __cplusplus
extern "C" {
#endif


struct ec_key_st;
typedef struct ec_key_st EC_KEY;
struct ec_group_st;
typedef struct ec_group_st EC_GROUP;
typedef struct ec_point_st EC_POINT;

EC_KEY *EC_KEY_new_by_curve_name(int nid);
void EC_KEY_free(EC_KEY *key);
int EC_KEY_generate_key(EC_KEY *key);
const EC_GROUP *EC_KEY_get0_group(const EC_KEY *key);
int EC_GROUP_get_degree(const EC_GROUP *group);


#ifdef __cplusplus
}
#endif
