#!/usr/bin/env sh

# code coverage script for travis CI

# generate coverage file
lcov --capture --ignore-errors gcov -b build --directory build --output-file build/coverage.info

# generate html report
genhtml build/coverage.info --output-directory build/report
