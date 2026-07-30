#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="build_perf"
REPORT_DIR="reports/perf"
EXECUTABLE="${BUILD_DIR}/regex_benchmarks"

mkdir -p "${REPORT_DIR}"

cmake -S . -B "${BUILD_DIR}" \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DCMAKE_CXX_FLAGS_RELWITHDEBINFO="-O2 -g -fno_omit_frame_pointer"

cmake --build "${BUILD_DIR}" --parallel "$(nproc)"

perf stat \
    --repeat 10
    -o "${REPORT_DIR}/perf_stat.txt" \
    -- "${EXECUTABLE}"

perf record \
    --call-graph dwarf \
    -o "${REPORT_DIR}/perf.data" \
    -- "${EXECUTABLE}"

perf report \
    --stdio \
    -i "${REPORT_DIR}/perf.data" \
    > "${REPORT_DIR}/perf_report.txt"