
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

#include "zip_filter.h"

namespace zipfilter {

// This is a convenience class that is used
// to store the data of a parsed zip file.
struct ZipSnapshot {
  struct FileEntry {
    // Local file record with header
    ByteSpan localFileRecord;
    records::CFHRecord cfhRecord;
    // Central directory record without header
    ByteSpan cfhData;
    std::string_view name;
  };

  records::EOCDRecord eocdRecord;
  std::vector<FileEntry> files;

  void writeToFile(const std::string &path);
};

void write(std::ofstream &out, ByteSpan span) {
  out.write(reinterpret_cast<char *>(span.data()), span.size());
}

void ZipSnapshot::writeToFile(const std::string &path) {
  std::ofstream out(path, std::ios::binary);
  size_t offset = 0;
  for (auto &file : files) {
    file.cfhRecord.relativeOffsetOfLocalHeader = offset;
    zipfilter::ByteSpan newSpan(file.localFileRecord.data(), file.localFileRecord.size() - 10);
    offset += file.localFileRecord.size();
    write(out, file.localFileRecord);
  }

  size_t size = 0;
  for (const auto &file : files) {
    const records::CFHRecord &record = file.cfhRecord;
    size += sizeof(record) + file.cfhData.size();
    out.write(reinterpret_cast<const char *>(&record), sizeof(record));
    write(out, file.cfhData);
  }

  eocdRecord.offsetToCdHeader = offset;
  eocdRecord.cdSize = size;
  eocdRecord.commentSize = 0;
  eocdRecord.diskEntries = files.size();
  eocdRecord.numEntries = files.size();
  out.write(reinterpret_cast<const char *>(&eocdRecord), sizeof(eocdRecord));
}

records::EOCDRecord *FindEOCDRecord(ByteSpan span) {
  constexpr int minEOCDSize = 22;
  std::byte *cursor = span.data() + span.size() - minEOCDSize;
  while (cursor != span.data()) {
    records::EOCDRecord *record = records::toEOCDRecord(cursor);
    if (record) {
      return record;
    }
    cursor--;
  }
  return nullptr;
}

std::expected<ZipFilter, ZipFilter::CreationError>
ZipFilter::FromPath(const std::string &path) {
  // Open file
  int fd = open(path.c_str(), O_RDONLY);
  if (fd == -1) {
    return std::unexpected(ZipFilter::CreationError::OpenFailed);
  }

  // Fetch size
  struct stat st;
  int ret = stat(path.c_str(), &st);
  if (ret == -1) {
    return std::unexpected(ZipFilter::CreationError::StatFailed);
  }
  size_t size = st.st_size;

  // Mmap file
  std::byte *start = (std::byte *)mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (start == MAP_FAILED) {
    return std::unexpected(ZipFilter::CreationError::MMapFailed);
  }

  close(fd);
  return ZipFilter({start, size});
}

ZipSnapshot Parse(ByteSpan span) {
  ZipSnapshot result;
  records::EOCDRecord *eocdRecord = FindEOCDRecord(span);
  if (!eocdRecord) {
    return result;
  }
  result.eocdRecord = *eocdRecord;

  std::byte *cdStart = span.data() + eocdRecord->offsetToCdHeader;
  std::byte *cursor = cdStart;
  std::byte *end = cdStart + eocdRecord->cdSize;
  while (cursor < end) {
    auto *cfhRecord = records::toCFHRecord(cursor);
    if (!cfhRecord) {
      return result;
    }
    std::byte *cfhRecordEnd = cursor + cfhRecord->size();
    std::byte *cfhDataStart = cursor + sizeof(records::CFHRecord);
    std::string_view name(reinterpret_cast<const char *>(cfhDataStart),
                          cfhRecord->fileNameLength);
    // std::cout << "Found record with name: " << name << std::endl;
    std::byte *lfhRecordStart =
        span.data() + cfhRecord->relativeOffsetOfLocalHeader;
    auto *lfhRecord = records::toLFHRecord(lfhRecordStart);
    if (!lfhRecord) {
      // std::cout << "Failed to locate lfh record" << std::endl;
    } else {
      std::byte *lfhRecordEnd = lfhRecordStart + lfhRecord->size();
      // Local file header with data
      ByteSpan lfhSpan = span.subspan(lfhRecordStart - span.data(),
                                      lfhRecordEnd - lfhRecordStart);
      // Central directory data without header
      ByteSpan cfhData =
          span.subspan(cfhDataStart - span.data(), cfhRecordEnd - cfhDataStart);
      result.files.emplace_back(lfhSpan, *cfhRecord, cfhData, name);
    }
    cursor = cfhRecordEnd;
  }
  return result;
}

void ZipFilter::Filter(const std::string &outPath, const std::string &suffix) {
  ZipSnapshot snapshot = Parse(span_);

  ZipSnapshot filtered;
  filtered.eocdRecord = snapshot.eocdRecord;
  for (const auto &file : snapshot.files) {
    if (file.name.ends_with(suffix)) {
      filtered.files.push_back(std::move(file));
    }
  }
  filtered.writeToFile(outPath);
}

std::vector<std::string> ZipFilter::ListFiles() {
  ZipSnapshot snapshot = Parse(span_);
  std::vector<std::string> result;
  for (const auto &file : snapshot.files) {
    std::string name(file.name.begin(), file.name.end());
    result.push_back(name);
  }
  return result;
}

} // namespace zipfilter