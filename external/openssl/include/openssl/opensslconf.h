/*
 * 精简源码树补丁：external/openssl 整包被裁，只有 recovery 编译真正用到的
 * openssl/sha.h + e_os2.h 被手动补齐（1.1.1w）。opensslconf.h 在上游是 Configure
 * 生成的产物，这里只需要让 e_os2.h 能过，且 SHA* 系列不在任何 OPENSSL_NO_* 守护下，
 * 所以给一个最小占位。
 */

#ifndef OPENSSL_CONF_H
# define OPENSSL_CONF_H
# include <stddef.h>
#endif
