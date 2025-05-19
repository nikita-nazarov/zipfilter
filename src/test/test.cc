
#include <array>
#include <cstdio>
#include <exception>
#include <filesystem>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "../lib/zip_filter.h"
#include "lib/file.h"
#include "lib/zip.h"

namespace {

struct ZipFilterParams {
  std::string archive;
  std::string suffix;
};

class DumpTest : public testing::TestWithParam<ZipFilterParams> {
private:
  void SetUp() override {
    tmpFilePath_ = test::CreateTempFile();
    tmpDirPath_ = test::CreateTempDir();
  }

  void TearDown() override {
    test::RemoveFile(tmpFilePath_);
    test::RemoveDir(tmpDirPath_);
  }

protected:
  std::string tmpFilePath_;
  std::string tmpDirPath_;
};

class ListFilesTest : public testing::TestWithParam<std::string> {};

} // namespace

std::unordered_set<std::string>
collectFilesWithSuffix(const std::string &zipPath, const std::string &suffix) {
  std::vector<std::string> allFiles = test::ZipInfo(zipPath);
  std::unordered_set<std::string> expectedSet;
  for (const std::string &file : allFiles) {
    if (file.ends_with(suffix)) {
      expectedSet.insert(std::move(file));
    }
  }
  return expectedSet;
}

TEST_P(ListFilesTest, AllFiles) {
  std::string archive = GetParam();
  zipfilter::ZipFilter filter = test::NewFilter(archive);
  std::vector<std::string> expected = test::ZipInfo(archive);
  std::vector<std::string> actual = filter.ListFiles();
  std::unordered_set<std::string> expectedSet(expected.begin(), expected.end());
  std::unordered_set<std::string> actualSet(actual.begin(), actual.end());
  ASSERT_EQ(expectedSet, actualSet);
}

TEST_P(DumpTest, ZipInfo) {
  auto [archive, suffix] = GetParam();
  std::unordered_set<std::string> expectedSet =
      collectFilesWithSuffix(archive, suffix);

  test::NewFilter(archive).Filter(tmpFilePath_, suffix);

  std::vector<std::string> actual = test::ZipInfo(tmpFilePath_);
  std::unordered_set<std::string> actualSet(actual.begin(), actual.end());
  ASSERT_EQ(expectedSet, actualSet);
}

TEST_P(DumpTest, Unzip) {
  // Test equal file names
  auto [archive, suffix] = GetParam();
  std::unordered_set<std::string> expectedFileNamesSet =
      collectFilesWithSuffix(archive, suffix);

  test::NewFilter(archive).Filter(tmpFilePath_, suffix);

  std::vector<std::string> actualFileNames =
      test::Unzip(tmpFilePath_, tmpDirPath_);
  std::unordered_set<std::string> actualFileNamesSet(actualFileNames.begin(),
                                                     actualFileNames.end());
  ASSERT_EQ(expectedFileNamesSet, actualFileNamesSet);

  // Test equal file contents
  std::vector<zipfilter::ByteSpan> actualContents;
  auto dirPath = std::filesystem::path(tmpDirPath_);
  for (const auto &fileName : actualFileNames) {
    actualContents.push_back(test::Read(dirPath / fileName));
  }

  // Replace previous files with unzipped from the test archive
  test::Unzip(archive, tmpDirPath_);

  for (size_t i = 0; i < actualFileNames.size(); i++) {
    zipfilter::ByteSpan expected = test::Read(dirPath / actualFileNames[i]);
    zipfilter::ByteSpan actual = actualContents[i];
    ASSERT_TRUE(std::equal(expected.begin(), expected.end(), actual.begin(),
                           actual.end()));
  }
}

INSTANTIATE_TEST_SUITE_P(ListFilesTests, ListFilesTest,
                         testing::Values("resources/uncompressed_dex.apk",
                                         "resources/compressed_dex.apk",
                                         "resources/images.zip"));

INSTANTIATE_TEST_SUITE_P(
    DumpTests, DumpTest,
    testing::Values(ZipFilterParams("resources/uncompressed_dex.apk", ".dex"),
                    ZipFilterParams("resources/compressed_dex.apk", ".dex")));
