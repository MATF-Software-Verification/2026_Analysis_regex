#!/usr/bin/env bash
set -euo pipefail

TOOL_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$TOOL_DIR/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build_sanitizers"
CFG_FILE="$TOOL_DIR/sanitizers.cfg"
REPORT_ROOT="$TOOL_DIR/reports"
TIMESTAMP="$(date +%d%m%Y_%H%M%S)"
REPORT_DIR="$REPORT_ROOT/run_${TIMESTAMP}"

if [[ -f "$CFG_FILE" ]]; then
    set -a
    source "$CFG_FILE"
    set +a
else
    echo "No sanitizers.cfg found."
    echo "cp sanitizers.cfg.example sanitizers.cfg"
    exit 1
fi
 
CXX="${CXX:-g++}"
JOBS="${JOBS:-$(nproc)}"

if [[ -z "${BOOST_ROOT:-}" || ! -d "$BOOST_ROOT" ]]; then 
    echo "BOOST_ROOT is not set or does not exist"
    exit 1
fi

mkdir -p "$REPORT_DIR"

cmake -S "$PROJECT_ROOT" -B "$BUILD_DIR" \
    -DBOOST_ROOT="$BOOST_ROOT" \
    -DBoost_NO_SYSTEM_PATHS=ON \
    -DBoost_NO_BOOST_CMAKE=ON \
    -DCMAKE_BUILD_TYPE="Debug" \
    -DCMAKE_CXX_COMPILER="$CXX" \
    -DENABLE_SANITIZERS="ON" \
    -DENABLE_COVERAGE="OFF"

cmake --build "$BUILD_DIR" --parallel "$JOBS"

ASAN_OPTIONS="halt_on_error=1:detect_stack_use_after_return=1" \
UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1" \
"$BUILD_DIR/regex_tests" > "$REPORT_DIR/regex_tests_output.log"

ASAN_OPTIONS="halt_on_error=1:detect_stack_use_after_return=1" \
UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1" \
"$BUILD_DIR/regex_app" 18 100 > "$REPORT_DIR/regex_app_output.log"