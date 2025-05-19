
.PHONY: build
build:
	 bazel build //:zipfilter

run:
	bazel run //:zipfilter -- resources/app.apk -d /tmp/archive.zip ".dex"

test:
	bazel test --test_output=all //:all