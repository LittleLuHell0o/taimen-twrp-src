#pragma once
//
// 精简树补丁：<ziparchive/zip_archive.h>（system/libziparchive 整包被裁）。
// 这里只做声明，实现来自 vendor/twrp/prebuilt_libs/libziparchive.so。
// 符号形态按 llvm-nm 输出对齐：
//   OpenArchive(char const*, ZipArchive**)
//   CloseArchive(ZipArchive*)
//   FindEntry(ZipArchive*, string_view, ZipEntry*)
//   ExtractEntryToFile(ZipArchive*, ZipEntry const*, int)
//

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include <sys/types.h>

struct ZipArchive;
typedef ZipArchive* ZipArchiveHandle;

typedef int64_t ZipEntry;
#define kInvalidZipEntry ((ZipEntry)-1)
#define kEndOfZipEntries ((ZipEntry)-2)

typedef int64_t ZipEntry64;
#define kInvalidZipEntry64 ((ZipEntry64)-1)
#define kEndOfZipEntries64 ((ZipEntry64)-2)

struct ZipEntryInfo {
  char name[128];
  uint32_t nameLen;
  uint16_t method;
  uint32_t crc32;
  int64_t compressed_size;
  int64_t uncompressed_size;
  int64_t offset;
  uint64_t access_time;
  uint64_t modify_time;
};

int OpenArchive(const char* zipFileName, ZipArchiveHandle* handle);
int OpenArchiveFd(int fd, const char* zipFileName, ZipArchiveHandle* handle, bool verify = false);
int OpenArchiveFdRange(int fd, const char* zipFileName, ZipArchiveHandle* handle, long offset,
                       long size, bool verify = false);
int OpenArchiveFromMemory(const void* address, size_t size, const char* zipFileName,
                          ZipArchiveHandle* handle);

void CloseArchive(ZipArchiveHandle handle);

ZipEntry FindEntry(ZipArchiveHandle handle, std::string_view entryName, ZipEntry* entry);
ZipEntry64 FindEntry64(ZipArchiveHandle handle, std::string_view entryName, ZipEntry64* entry);

bool ExtractEntryToFile(ZipArchiveHandle handle, ZipEntry* entry, int fd);
bool ExtractEntryToFile(ZipArchiveHandle handle, ZipEntry64* entry, int fd);
bool ExtractEntryToBuffer(ZipArchiveHandle handle, ZipEntry* entry, std::vector<uint8_t>* buffer);

bool Next(ZipArchiveHandle handle, ZipEntry* entry, std::string* name);
bool Next64(ZipArchiveHandle handle, ZipEntry64* entry, std::string* name);

void CloseEntry(ZipArchiveHandle handle, ZipEntry entry);

int GetZipEntryOffset(ZipArchiveHandle handle, ZipEntry entry, off64_t* offset);
int GetZipEntryUncompressedSize(ZipArchiveHandle handle, ZipEntry entry, uint64_t* size);
int GetZipEntryFileName(ZipArchiveHandle handle, ZipEntry entry, std::string* name);
int GetZipEntryCompressedMethod(ZipArchiveHandle handle, ZipEntry entry, uint16_t* method);
