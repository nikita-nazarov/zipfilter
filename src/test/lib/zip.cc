
#include <array>
#include <cstdio>
#include <filesystem>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>

#include "zip.h"

namespace test {

std::string Exec(const char *cmd) {
  std::array<char, 128> buffer;
  std::string result;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
  if (!pipe) {
    throw std::runtime_error("popen() failed!");
  }
  while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) !=
         nullptr) {
    result += buffer.data();
  }
  return result;
}

std::vector<std::string> ZipInfo(const std::string &path) {
  std::string output = Exec(("zipinfo -1 " + path).c_str());
  std::stringstream ss(output);
  std::string line;
  std::vector<std::string> result;
  while (std::getline(ss, line, '\n')) {
    result.push_back(std::move(line));
  }
  return result;
}

std::vector<std::string> Unzip(const std::string &path,
                               const std::string &dstPath) {
  Exec(("unzip -q -o " + path + " -d " + dstPath).c_str());
  std::vector<std::string> files;
  for (const auto &entry :
       std::filesystem::recursive_directory_iterator(dstPath)) {
    files.push_back(entry.path().filename().string());
  }
  return files;
}

zipfilter::ZipFilter NewFilter(const std::string &path) {
  auto res = zipfilter::ZipFilter::FromPath(path);
  if (!res.has_value()) {
    throw std::runtime_error("Failed to create zip archive");
  }
  return res.value();
}

} // namespace test
