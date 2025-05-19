load("@rules_cc//cc:cc_binary.bzl", "cc_binary")
load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")

cc_library(
    name = "zipfilter_lib",
    srcs = glob(["src/lib/**/*.cc"]),
    hdrs = glob(["src/lib/**/*.h"]),
)

test_archives = [
    "resources/uncompressed_dex.apk",
    "resources/compressed_dex.apk",
    "resources/images.zip",
]

cc_binary(
    name = "zipfilter",
    srcs = ["src/main.cc"],
    data = test_archives,
    deps = [":zipfilter_lib"],
)

cc_library(
    name = "zipfilter_test_lib",
    srcs = glob(["src/test/lib/**/*.cc"]),
    hdrs = glob(["src/test/lib/**/*.h"]),
    deps = [
        ":zipfilter_lib",
    ],
)

cc_test(
    name = "zipfilter_test",
    size = "small",
    srcs = [
        "src/test/test.cc",
    ],
    data = test_archives,
    deps = [
        ":zipfilter_lib",
        ":zipfilter_test_lib",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "cli_test",
    size = "small",
    srcs = [
        "src/test/cli_test.cc",
    ],
    data = test_archives,
    deps = [
        ":zipfilter",
        "@googletest//:gtest",
        "@googletest//:gtest_main",
    ],
)
