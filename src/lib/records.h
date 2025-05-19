#pragma once

#include <cstddef>
#include <cstdint>
#include <span>

// TODO add zip64 support

namespace zipfilter {
namespace records {

// End of central directory
constexpr int EOCD_SIGNATURE = 0x06054b50;
// Central file header
constexpr int CFH_SIGNATURE = 0x02014b50;
// Local file header
constexpr int LFH_SIGNATURE = 0x04034b50;

struct __attribute__((packed)) LFHRecord {
  uint32_t signature;
  uint16_t versionNeeded;
  uint16_t generalPurposeBitFlag;
  uint16_t compressionMethod;
  uint16_t lastModFileTime;
  uint16_t lastModFileDate;
  uint32_t crc32;
  uint32_t compressedSize;
  uint32_t uncompressedSize;
  uint16_t fileNameLength;
  uint16_t extraFieldLength;

  size_t size();
};

// Data descriptor. It is only present if bit 3 of
// a local fiel header bit flag is set.
struct __attribute__((packed)) DDRecord {
  uint32_t crc32;
  uint32_t compressedSize;
  uint32_t uncompressedSize;
};

struct __attribute__((packed)) CFHRecord {
  uint32_t signature;
  uint16_t version;
  uint16_t versionNeeded;
  uint16_t generalPurposeBitFlag;
  uint16_t compressionMethod;
  uint16_t lastModFileTime;
  uint16_t lastModFileDate;
  uint32_t crc32;
  uint32_t compressedSize;
  uint32_t uncompressedSize;
  uint16_t fileNameLength;
  uint16_t extraFieldLength;
  uint16_t fileCommentLength;
  uint16_t diskNumberStart;
  uint16_t internalFileAttributes;
  uint32_t externalFileAttributes;
  uint32_t relativeOffsetOfLocalHeader;

  size_t size();
};

struct __attribute__((packed)) EOCDRecord {
  uint32_t signature;
  uint16_t diskNumber;
  uint16_t numDisk;
  uint16_t diskEntries;
  uint16_t numEntries;
  uint32_t cdSize;
  uint32_t offsetToCdHeader;
  uint16_t commentSize;
};

EOCDRecord *toEOCDRecord(std::byte *ptr);

LFHRecord *toLFHRecord(std::byte *ptr);

CFHRecord *toCFHRecord(std::byte *ptr);

} // namespace records
} // namespace zipfilter