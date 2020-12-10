# ONE Game Hosting SDK Tests

All tests for the SDK are contained and managed here.

## Catch2

The excellent external Catch2 testing framework is used.

https://github.com/catchorg/Catch2

tag: `v2.12.2`
Commit: `b1b5cb812277f367387844aab46eb2d3b15d03cd`

## File Structure

### Unit Tests

Do one of the following:

1. Name the file after the corresponding component folder within the one folder. For example "tests/arcus.cpp" if adding a test for something within "one/arcus/..".
2. Name the file after a header that is being tests, e.g. "ring.cpp" if testing "one/arcus/internal/ring.h".

### Integration Tests

1. If the testing a Fake Game with a Fake Agent, add it to tests/integration.cpp.
2. Integration tests should only reference public interfaces. No friend classes for testing purposes or touching of /internal folders.

### Utilities

Test utilities go in tests/util.h.
