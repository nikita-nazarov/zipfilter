
#include <cstdint>
#include <filesystem>
#include <gtest/gtest.h>

#include "src/test/lib/file.h"
#include "src/test/lib/zip.h"

namespace {

// Helper struct to remove a file at the end of its scope.
struct FileRemover {
  std::string path_;
  FileRemover(std::string path) : path_(std::move(path)) {}
  ~FileRemover() {
    try {
      std::filesystem::remove(path_);
    } catch (const std::filesystem::filesystem_error& e) {
      // Ignore errors, e.g., file not found.
    }
  }
};

// Helper struct to remove a directory recursively at the end of its scope.
struct DirRemover {
  std::string path_;
  DirRemover(std::string path) : path_(std::move(path)) {}
  ~DirRemover() {
    try {
      std::filesystem::remove_all(path_);
    } catch (const std::filesystem::filesystem_error& e) {
      // Ignore errors, e.g., directory not found.
    }
  }
};

const std::string ZIPFILTER = "zipfilter";
const std::string INPUT_ARCHIVE = "resources/uncompressed_dex.apk";
const std::string INPUT_ARCHIVE_COMPRESSED = "resources/compressed_dex.apk";
const std::string OUTPUT_ARCHIVE_PATH = "/tmp/archive.zip";
const std::string SUFFIX_TO_EXTRACT = ".dex";

int Zipfilter(const std::string &args) {
  return std::system((ZIPFILTER + " " + args).c_str());
}

} // namespace

TEST(CliTest, WrongArgs) {
  ASSERT_NE(Zipfilter(""), 0);
  ASSERT_NE(Zipfilter("-d /tmp/archive.zip"), 0);
  ASSERT_NE(Zipfilter(INPUT_ARCHIVE), 0);
  ASSERT_NE(Zipfilter("'.dex'"), 0);
  ASSERT_NE(Zipfilter(INPUT_ARCHIVE + " '.dex' -d"), 0);
  ASSERT_NE(Zipfilter("-d " + INPUT_ARCHIVE + " '.dex'"), 0);
}

TEST(CliTest, CorrectArgs) {
  ASSERT_EQ(Zipfilter(INPUT_ARCHIVE + " '.dex'"), 0);
  ASSERT_EQ(Zipfilter(INPUT_ARCHIVE + " '.dex' -d /tmp/archive.zip"), 0);
  ASSERT_EQ(Zipfilter(INPUT_ARCHIVE + " -d /tmp/archive.zip '.dex'"), 0);
  ASSERT_EQ(Zipfilter("-d /tmp/archive.zip " + INPUT_ARCHIVE + " '.dex'"), 0);
}

TEST(CliTest, ExtractDexFiles) {
  FileRemover output_archive_remover(OUTPUT_ARCHIVE_PATH); // Remove on scope exit

  int exit_code = Zipfilter(INPUT_ARCHIVE_COMPRESSED + " '" + SUFFIX_TO_EXTRACT + "' -d " + OUTPUT_ARCHIVE_PATH);
  ASSERT_EQ(exit_code, 0);

  std::string temp_extraction_path = test::CreateTempDir();
  DirRemover temp_dir_remover(temp_extraction_path); // Remove on scope exit

  std::vector<std::string> unzipped_files = test::Unzip(OUTPUT_ARCHIVE_PATH, temp_extraction_path);

  ASSERT_FALSE(unzipped_files.empty());
  for (const auto& file_name : unzipped_files) {
    ASSERT_TRUE(file_name.ends_with(SUFFIX_TO_EXTRACT));
  }
}
