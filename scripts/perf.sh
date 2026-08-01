#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
CFG_FILE="$PROJECT_ROOT/perf.cfg"
BUILD_DIR="$PROJECT_ROOT/build_perf"
REPORT_ROOT="$PROJECT_ROOT/reports/perf"
TIMESTAMP="$(date +%d%m%Y_%H%M%S)"
REPORT_DIR="$REPORT_ROOT/run_${TIMESTAMP}"

if [[ -f "$CFG_FILE" ]]; then
    set -a
    source "$CFG_FILE"
    set +a
else 
    echo "No perf.cfg found."
    echo "cp perf.cfg.example perf.cfg and fill in your local values"
    exit 1
fi

BUILD_TYPE="${BUILD_TYPE:-RelWithDebInfo}"
CXX="${CXX:-g++}"
JOBS="${JOBS:-$(nproc)}"
PERF_TARGET="${PERF_TARGET:-regex_tests}"
PERF_STAT_REPEAT="${PERF_STAT_REPEAT:-5}"
PERF_FREQUENCY="${PERF_FREQUENCY:-999}"
PERF_CALL_GRAPH="${PERF_CALL_GRAPH:-dwarf}"
PERF_PERCENT_LIMIT="${PERF_PERCENT_LIMIT:-0.5}"

if [[ -z "${BOOST_ROOT:-}" || ! -d "$BOOST_ROOT" ]]; then
    echo "BOOST_ROOT is not set or does not exist"
    exit 1
fi

if ! command -v "$CXX" &>/dev/null; then
    echo "${CXX} not found"
    exit 1
fi

if ! command -v perf &>/dev/null; then
    echo "perf not found"
    exit 1
fi

mkdir -p "${REPORT_DIR}"

cmake -S "${PROJECT_ROOT}" -B "${BUILD_DIR}" \
    -DBOOST_ROOT="$BOOST_ROOT" \
    -DBoost_NO_SYSTEM_PATHS=ON \
    -DBoost_NO_BOOST_CMAKE=ON \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCMAKE_CXX_FLAGS="-fno-omit-frame-pointer" \
    -DCMAKE_CXX_COMPILER="$CXX" \
    -DENABLE_ASAN=OFF \
    -DENABLE_COVERAGE=OFF \

cmake --build "${BUILD_DIR}" --parallel "$JOBS"

TARGET_ARGS=()

if declare -p PERF_TARGET_ARGS &>/dev/null; then
    TARGET_ARGS=("${PERF_TARGET_ARGS[@]}")
fi

EXECUTABLE="$BUILD_DIR/$PERF_TARGET"

perf stat \
    --repeat "$PERF_STAT_REPEAT" \
    --output "$REPORT_DIR/perf_stat.txt" \
    -- "${EXECUTABLE}" "${TARGET_ARGS[@]}" \
    > "$REPORT_DIR/perf_stat_output.log"

perf record \
    --freq "$PERF_FREQUENCY" \
    --call-graph "$PERF_CALL_GRAPH" \
    --output "$REPORT_DIR/perf.data" \
    -- "${EXECUTABLE}" "${TARGET_ARGS[@]}" \
    > "$REPORT_DIR/perf_record_output.log"

perf report \
    --stdio \
    --input "$REPORT_DIR/perf.data" \
    --dsos "$PERF_TARGET" \
    --no-children \
    --call-graph none \
    --sort symbol \
    --percent-limit "$PERF_PERCENT_LIMIT" \
    --show-nr-samples \
    > "$REPORT_DIR/perf_report.txt"