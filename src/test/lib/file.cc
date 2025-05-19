
#include <cstdio>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../../lib/zip_filter.h"
#include "file.h"

namespace test {

std::string CreateTempFile() {
  std::string path = std::tmpnam(nullptr);
  std::ofstream out(path);
  return path;
}

std::string CreateTempDir() {
  std::string path = std::tmpnam(nullptr);
  if (!std::filesystem::create_directory(path)) {
    throw std::runtime_error("Couldn't create directory " + path);
  }
  return path;
}

void RemoveFile(const std::string &path) {
  if (!std::filesystem::remove(path)) {
    throw std::runtime_error("Couldn't remove file " + path);
  }
}

void RemoveDir(const std::string &path) {
  if (!std::filesystem::remove_all(path)) {
    throw std::runtime_error("Couldn't remove directory " + path);
  }
}

zipfilter::ByteSpan Read(const std::string &path) {
  // Open file
  int fd = open(path.c_str(), O_RDONLY);
  if (fd == -1) {
    throw std::runtime_error("Couldn't open file " + path);
  }

  // Fetch size
  struct stat st;
  int ret = stat(path.c_str(), &st);
  if (ret == -1) {
    throw std::runtime_error("Couldn't fetch size of " + path);
  }
  size_t size = st.st_size;

  // Mmap file
  std::byte *start = (std::byte *)mmap(0, size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (start == MAP_FAILED) {
    throw std::runtime_error("Couldn't mmap " + path);
  }

  close(fd);
  return zipfilter::ByteSpan(start, size);
}

} // namespace test