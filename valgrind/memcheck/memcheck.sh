#!/usr/bin/env bash
set -euo pipefail

TOOL_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$TOOL_DIR/../.." && pwd)"
CFG_FILE="$TOOL_DIR/memcheck.cfg"
BUILD_DIR="$PROJECT_ROOT/build_memcheck"
REPORT_DIR="$TOOL_DIR/reports"
TIMESTAMP="$(date +%d%m%Y_%H%M%S)"
LOG_FILE="$REPORT_DIR/memcheck_regex_tests_${TIMESTAMP}.log"

if [[ -f "$CFG_FILE" ]]; then
    set -a
    source "$CFG_FILE"
    set +a
else
    echo "No memcheck.cfg file found"
    exit 1
fi

BOOST_TYPE="${BUILD_TYPE:-Debug}"
CXX="${CXX:-g++}"
JOBS="${JOBS:-$(nproc)}"
VALGRIND_TARGET="${VALGRIND_TARGET:-regex_tests}"

if [[ -z "${BOOST_ROOT:-}" || ! -d "$BOOST_ROOT" ]]; then 
    echo "BOOST_ROOT is not set or does not exist"
    exit 1
fi

if ! command -v "$CXX" &>/dev/null; then
    echo "${CXX} not found"
    exit 1
fi

if ! command -v valgrind &>/dev/null; then
    echo "Valgrind not found"
    exit 1
fi

mkdir -p "$REPORT_DIR"

cmake -S "$PROJECT_ROOT" -B "$BUILD_DIR" \
    -DBOOST_ROOT="$BOOST_ROOT" \
    -DBoost_NO_SYSTEM_PATHS=ON \
    -DBoost_NO_BOOST_CMAKE=ON \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCMAKE_CXX_COMPILER="$CXX" \
    -DENABLE_ASAN=OFF \
    -DENABLE_COVERAGE=OFF

cmake --build "$BUILD_DIR" --parallel "$JOBS"

valgrind \
    --tool=memcheck \
    --leak-check=full \
    --show-leak-kinds=definite,indirect,possible \
    --track-origins=yes \
    --num-callers=30 \
    --error-exitcode=101 \
    --log-file="$LOG_FILE" \
    "$BUILD_DIR/$VALGRIND_TARGET"

echo "Valgrind Memcheck report: $LOG_FILE"
