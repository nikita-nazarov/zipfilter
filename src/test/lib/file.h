#pragma once

#include <string>

namespace test {

std::string CreateTempFile();
std::string CreateTempDir();
void RemoveFile(const std::string &path);
void RemoveDir(const std::string &path);

// Reads whole contents of a file to a byte span
zipfilter::ByteSpan Read(const std::string &path);

} // namespace test
