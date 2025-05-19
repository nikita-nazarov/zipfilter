#pragma once

#include <string>
#include <vector>

#include "../../lib/zip_filter.h"

namespace test {

// Returns a list of files in an archive from `zipinfo`
std::vector<std::string> ZipInfo(const std::string &path);

// Unzips an archive to a directory and returns a list
// of files in this directory
std::vector<std::string> Unzip(const std::string &path,
                               const std::string &dstPath);

zipfilter::ZipFilter NewFilter(const std::string &path);

} // namespace test
