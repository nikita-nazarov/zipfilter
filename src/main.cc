
#include <fstream>
#include <iostream>
#include <string>

#include "lib/zip_filter.h"

namespace {

const char *DEFAULT_OUT_FILE = "archive.zip";

void PrintUsage() {
  std::cout << "Usage:" << std::endl;
  std::cout << "zipfilter [zip archive path] [pattern] [-d output]"
            << std::endl;
}

struct Args {
  const char *zipPath = nullptr;
  const char *outFile = nullptr;
  const char *suffix = nullptr;
};

std::expected<Args, std::string> ParseArgs(int argc, char *argv[]) {
  Args args;
  for (size_t i = 1; i < argc; i++) {
    const char *arg = argv[i];
    if (!std::strcmp(arg, "-d")) {
      if (i == argc - 1) {
        return std::unexpected("Missing output file path");
      }
      args.outFile = argv[++i];
    } else if (!args.zipPath) {
      args.zipPath = arg;
    } else {
      args.suffix = arg;
    }
  }

  if (!args.zipPath) {
    return std::unexpected("Missing zip archive path");
  }

  if (!args.suffix) {
    return std::unexpected("Missing filter suffix");
  }

  if (!args.outFile) {
    args.outFile = DEFAULT_OUT_FILE;
  }
  return args;
}

} // namespace

int main(int argc, char *argv[]) {
  auto res = ParseArgs(argc, argv);
  if (!res.has_value()) {
    std::cerr << res.error() << std::endl;
    PrintUsage();
    return 1;
  }
  Args args = res.value();

  auto creationRes = zipfilter::ZipFilter::FromPath(args.zipPath);
  if (!creationRes.has_value()) {
    int error = static_cast<int>(creationRes.error());
    std::cerr << "Error when creating zip archive: " << std::to_string(error)
              << std::endl;
    return 1;
  }
  zipfilter::ZipFilter archive = creationRes.value();

  archive.Filter(args.outFile, args.suffix);
  return 0;
}
