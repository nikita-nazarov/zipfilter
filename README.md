# Zipfilter
Zipfilter is a library that takes a zip file as an input and returns a new archive with files that satisfy a given pattern.


For example you can extract all `.dex` files of an `apk` like this: 
```
> zipfilter my.apk ".dex" -d archive.zip
```

## Building
The project is built and tested with Bazel.

```
make build
```

## Running tests

```
make test
```
