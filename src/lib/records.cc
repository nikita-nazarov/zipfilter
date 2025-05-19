
#include "records.h"

namespace zipfilter {
namespace records {

template <class T, int Signature> T *toRecord(std::byte *ptr) {
  T *record = reinterpret_cast<T *>(ptr);
  if (record->signature != Signature) {
    return nullptr;
  }
  return record;
}

size_t CFHRecord::size() {
  return sizeof(records::CFHRecord) + fileNameLength + extraFieldLength +
         fileCommentLength;
}

bool isDataDescriptorPresent(LFHRecord *lfhRecord) {
  // TODO implement
  return false;
}

size_t LFHRecord::size() {
  size_t ddSize = isDataDescriptorPresent(this) ? sizeof(DDRecord) : 0;
  return sizeof(records::LFHRecord) + fileNameLength + extraFieldLength +
         compressedSize + ddSize;
}

EOCDRecord *toEOCDRecord(std::byte *ptr) {
  return toRecord<EOCDRecord, EOCD_SIGNATURE>(ptr);
}

LFHRecord *toLFHRecord(std::byte *ptr) {
  return toRecord<LFHRecord, LFH_SIGNATURE>(ptr);
}

CFHRecord *toCFHRecord(std::byte *ptr) {
  return toRecord<CFHRecord, CFH_SIGNATURE>(ptr);
}

} // namespace records
} // namespace zipfilter