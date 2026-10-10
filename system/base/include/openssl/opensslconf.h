#pragma once
//
// 精简树补丁：<openssl/opensslconf.h>。
// 本树不编译 openssl，recovery 只用到 BN/EC/EVP/PEM 这几个子集，
// 全部走声明式桩位 + prebuilt libcrypto.so，所以这里给一份空的开关即可。
//
