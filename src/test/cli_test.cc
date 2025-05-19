
#include <cstdint>
#include <gtest/gtest.h>

namespace {

const std::string ZIPFILTER = "zipfilter";
const std::string INPUT_ARCHIVE = "resources/uncompressed_dex.apk";

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
