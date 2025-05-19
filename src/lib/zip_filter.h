#pragma once

#include <cstddef>
#include <expected>
#include <span>
#include <string>
#include <vector>

#include "records.h"

namespace zipfilter {

using ByteSpan = std::span<std::byte>;

class ZipFilter {
public:
  enum class CreationError { OpenFailed, StatFailed, MMapFailed };

  static std::expected<ZipFilter, CreationError>
  FromPath(const std::string &path);

  std::vector<std::string> ListFiles();

  void Filter(const std::string &outPath, const std::string &suffix);

private:
  ZipFilter(ByteSpan span) : span_(span) {}

private:
  ByteSpan span_;
};

} // namespace zipfilter
