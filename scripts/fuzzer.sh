#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
CFG_FILE="$PROJECT_ROOT/fuzztest.cfg"
FUZZ_DIR="$PROJECT_ROOT/fuzztest"
BUILD_DIR="$PROJECT_ROOT/build_fuzz"
LOG_ROOT="$FUZZ_DIR/log"
SUMMARY_ROOT="$FUZZ_DIR/summary"

if [[ -f "$CFG_FILE" ]]; then
    set -a
    source "$CFG_FILE"
    set +a
else
    echo "No fuzzer.cfg file found"
    exit 1
fi

BUILD_TYPE="${BUILD_TYPE:-Debug}"
CXX="${CXX:-clang++}"
CC="${CC:-clang}"
JOBS="${JOBS:-$(nproc)}"
FUZZ_DURATION="${FUZZ_DURATION:-60}"


if [[ -z "${BOOST_ROOT:-}"  || ! -d "$BOOST_ROOT" ]]; then
    echo "BOOST_ROOT is not set or does not exist"
    exit 1
fi

if ! command -v "$CXX" &>/dev/null; then
    echo "${CXX} not found"
    exit 1
fi

if ! command -v "$CC" &>/dev/null; then
    echo "${CC} (C compiler) not found"
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
    -DCMAKE_CXX_COMPILER="$CXX" \
    -DCMAKE_CC_COMPILER="$CC" \
    -DFUZZTEST_FUZZING_MODE=on

cmake --build "$BUILD_DIR" --parallel "${JOBS}" 

mkdir -p "$LOG_ROOT" "$SUMMARY_ROOT"

for t in "${TARGETS[@]}"; do
    NAME="${t//./_}"
    SUMMARY_FILE="$SUMMARY_ROOT/${NAME}_$(date +%d%m%Y_%H%M%S).log"
    LOG_FILE="$LOG_ROOT/log_${NAME}.raw"

    echo "================================================================="
    echo "=== Fuzzing $t"
    stdbuf -oL -eL "$BUILD_DIR/fuzz_regex" \
    --fuzz="$t" \
    --fuzz_for="${FUZZ_DURATION}s" \
    --stack_limit_kb=4069 2>&1 | tee "$LOG_FILE" || true

    grep -v '^\[\*\]' "$LOG_FILE" > "$SUMMARY_FILE"
done