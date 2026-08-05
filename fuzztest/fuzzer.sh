#!/usr/bin/env bash

set -euo pipefail

FUZZ_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$FUZZ_DIR/.." && pwd)"
CFG_FILE="$FUZZ_DIR/fuzztest.cfg"
BUILD_DIR="$PROJECT_ROOT/build_fuzz"
REPORT_ROOT="$FUZZ_DIR/reports"
TIMESTAMP="$(date +%d%m%Y_%H%M%S)"
REPORT_DIR="$REPORT_ROOT/run_$TIMESTAMP"

mkdir -p "$REPORT_DIR"

# exec > >(tee -a "$REPORT_DIR/test_discovery.log") 2>&1
# for run in $(seq 1 200); do
#     echo "Discovery run ${run}"
#     ASAN_OPTIONS="detect_leaks=1" \
#     LSAN_OPTIONS="verbosity=1:log_threads=1" \
#     "$BUILD_DIR/fuzz_regex" --gtest_list_tests >/dev/null || {
#         echo "Crashed during test discovery"
#         break
#     }
# done

if [[ -f "$CFG_FILE" ]]; then
    set -a
    source "$CFG_FILE"
    set +a
else
    echo "No fuzztest.cfg file found"
    exit 1
fi

BUILD_TYPE="${BUILD_TYPE:-Debug}"
JOBS="${JOBS:-$(nproc)}"
FUZZ_DURATION="${FUZZ_DURATION:-60}"


if [[ -z "${BOOST_ROOT:-}"  || ! -d "$BOOST_ROOT" ]]; then
    echo "BOOST_ROOT is not set or does not exist"
    exit 1
fi

if [[ -n "${FUZZ_TARGETS:-}" ]]; then
    read -ra TARGETS <<< "$FUZZ_TARGETS"
    echo "Using targets from config: ${TARGETS[*]}"
else 
    mapfile -t TARGETS < <(
        grep -oP 'FUZZ_TEST\(\K[^)]+' "${FUZZ_DIR}/fuzz_regex.cc" |
        sed 's/[[:space:]]*,[[:space:]]*/./'
    )
    echo "Discovered targets: ${TARGETS[*]}"
fi


export ASAN_OPTIONS="alloc_dealloc_mismatch=0" #due to libc++abi.so library implementation of exception classes 

cmake -S "$FUZZ_DIR" -B "$BUILD_DIR" \
    -DBOOST_ROOT="$BOOST_ROOT" \
    -DBoost_NO_SYSTEM_PATHS=ON \
    -DBoost_NO_BOOST_CMAKE=ON \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_C_COMPILER=clang \
    -DFUZZTEST_FUZZING_MODE=on

cmake --build "$BUILD_DIR" --parallel "${JOBS}" 

LOG_FILE="$REPORT_DIR/output.log"

exec > >(tee -a "$LOG_FILE") 2>&1


for t in "${TARGETS[@]}"; do
    ASAN_LOG="${t//./_}_asan.log"
    echo "================================================================="
    echo "=== Fuzzing $t"
    "$BUILD_DIR/fuzz_regex" \
    --fuzz="$t" \
    --fuzz_for="${FUZZ_DURATION}s" \
    --stack_limit_kb=4096

done