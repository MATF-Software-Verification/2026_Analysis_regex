#!/usr/bin/env bash
set -euo pipefail

TOOL_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$TOOL_DIR/.." && pwd)"
CFG_FILE="$TOOL_DIR/mull.cfg"
BUILD_DIR="$PROJECT_ROOT/build_mull"
REPORT_ROOT="$TOOL_DIR/reports"
TIMESTAMP="$(date +%d%m%Y_%H%M%S)"
REPORT_DIR="$REPORT_ROOT/run_${TIMESTAMP}"

if [[ -f "$CFG_FILE" ]]; then
    set -a
    source "$CFG_FILE"
    set +a
else
    echo "No mull.cfg file found"
    echo "Copy mull.cfg.example to mull.cfg and set BOOST_SOURCE_ROOT"
    exit 1
fi

LLVM_VERSION="$("clang++" --version | sed -n 's/.*clang version \([0-9][0-9]*\).*/\1/p')"

MULL_RUNNER="mull-runner-${LLVM_VERSION}"
MULL_PLUGIN="/usr/lib/mull-ir-frontend-${LLVM_VERSION}"
MULL_TARGET="regex_tests"
MULL_TIMEOUT_MS="${MULL_TIMEOUT_MS:-10000}"
MULL_WORKERS="${MULL_WORKERS:-$(nproc)}"
JOBS="${JOBS:-$(nproc)}"
LD_SEARCH_PATH="${LD_SEARCH_PATH:-usr/lib/}"

if [[ -z "${BOOST_ROOT:-}" || ! -d "$BOOST_ROOT" ]]; then
    echo "BOOST_ROOT is not set or does not exist"
    exit 1
fi

if ! command -v "$MULL_RUNNER" &>/dev/null; then
    echo "$MULL_RUNNER not found"
    exit 1
fi

export MULL_CONFIG="$TOOL_DIR/mull.yml"

rm -rf "$BUILD_DIR"
mkdir -p "$REPORT_DIR"

MULL_COMPILE_FLAGS="-O0 -g -grecord-command-line -fpass-plugin=$MULL_PLUGIN"

cmake -S "$PROJECT_ROOT" -B "$BUILD_DIR" \
    -DBOOST_ROOT="$BOOST_ROOT" \
    -DBoost_NO_SYSTEM_PATHS=ON \
    -DBoost_NO_BOOST_CMAKE=ON \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_COMPILER=clang++ \
    -DCMAKE_C_COMPILER=clang \
    -DCMAKE_CXX_FLAGS="$MULL_COMPILE_FLAGS" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DCMAKE_NO_SYSTEM_FROM_IMPORTED=ON

cmake --build "$BUILD_DIR" --target "$MULL_TARGET" --parallel "$JOBS"

"$MULL_RUNNER" \
    --workers "$MULL_WORKERS" \
    --timeout "$MULL_TIMEOUT_MS" \
    --allow-surviving \
    --reporters IDE \
    --reporters Elements \
    --report-dir "$REPORT_DIR" \
    --report-name mull_reports \
    --ld-search-path "$LD_SEARCH_PATH" \
    "$BUILD_DIR/$MULL_TARGET" \
    > "$REPORT_DIR/output.log" 2>&1

echo ""
echo "Mull reports: $REPORT_DIR"