#!/usr/bin/env bash
set -euo pipefail
 
TESTS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$TESTS_DIR/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"
COVERAGE_DIR="$TESTS_DIR/reports"
CFG_FILE="$TESTS_DIR/coverage.cfg"
 
if [[ -f "$CFG_FILE" ]]; then
    set -a
    source "$CFG_FILE"
    set +a
else
    echo "No coverage.cfg found."
    echo "cp project.cfg.example coverage.cfg and set BOOST_ROOT to local Boost instalattion"
    exit 1
fi
 
BUILD_TYPE="${BUILD_TYPE:-Debug}"
CXX="${CXX:-g++}"
JOBS="${JOBS:-$(nproc)}"

if [[ -z "${BOOST_ROOT:-}"  || ! -d "$BOOST_ROOT" ]]; then
    echo "BOOST_ROOT is not set or does not exist"
    exit 1
fi

rm -rf "$BUILD_DIR"
rm -rf "$COVERAGE_DIR"
mkdir "$COVERAGE_DIR"
 
cmake -S "$PROJECT_ROOT" -B "$BUILD_DIR" \
    -DBOOST_ROOT="$BOOST_ROOT" \
    -DBoost_NO_SYSTEM_PATHS=ON \
    -DBoost_NO_BOOST_CMAKE=ON \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCMAKE_CXX_COMPILER="$CXX" \
    -DENABLE_SANITIZERS=ON \
    -DENABLE_COVERAGE=ON
 
cmake --build "$BUILD_DIR" --parallel "$JOBS"

cd "$BUILD_DIR"
ctest -V | tee "$COVERAGE_DIR/output.log" || true

lcov --capture \
    --directory "$BUILD_DIR" \
    --output-file "$COVERAGE_DIR/coverage_raw.info" \
    --rc lcov_branch_coverage=1

lcov --extract "$COVERAGE_DIR/coverage_raw.info" \
    "*/boost/regex*" \
    --output-file "$COVERAGE_DIR/coverage_regex.info" \
    --rc lcov_branch_coverage=1

genhtml "$COVERAGE_DIR/coverage_regex.info" \
    --output-directory "$COVERAGE_DIR/html" \
    --branch-coverage \
    --rc genhtml_branch_coverage=1 \
    --ignore-errors source \

echo ""
echo "Coverage report: $COVERAGE_DIR/html/index.html"
xdg-open "$COVERAGE_DIR/html/index.html"