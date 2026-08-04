#!/usr/bin/env bash
set -euo pipefail

TOOL_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$TOOL_DIR/../.." && pwd)"
CFG_FILE="$TOOL_DIR/memcheck.cfg"
BUILD_DIR="$PROJECT_ROOT/build_memcheck"
REPORT_ROOT="$TOOL_DIR/reports"
TIMESTAMP="$(date +%d%m%Y_%H%M%S)"
REPORT_DIR="$REPORT_ROOT/run_$TIMESTAMP"
LOG_FILE="$REPORT_DIR/memcheck_regex_tests.log"

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
    -DENABLE_SANITIZERS=OFF \
    -DENABLE_COVERAGE=OFF

cmake --build "$BUILD_DIR" --parallel "$JOBS"

valgrind \
    --tool=memcheck \
    --leak-check=full \
    --show-leak-kinds=all \
    --track-origins=yes \
    --num-callers=30 \
    --log-file="$LOG_FILE" \
    "$BUILD_DIR/regex_tests" | tee "$REPORT_DIR/output.log"
echo "Valgrind Memcheck report: $LOG_FILE"
