#!/usr/bin/env bash
set -euo pipefail

TOOL_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CFG_FILE="$TOOL_DIR/cppcheck.cfg"
REPORT_ROOT="$TOOL_DIR/reports"
TIMESTAMP="$(date +%d%m%Y_%H%M%S)"
REPORT_DIR="$REPORT_ROOT/run_${TIMESTAMP}"
XML_FILE="$REPORT_DIR/cppcheck.xml"

if [[ -f "$CFG_FILE" ]]; then
    set -a
    source "$CFG_FILE"
    set +a
else 
    echo "No cppcheck.cfg found."
    echo "cp cppcheck.cfg.example cppcheck.cfg and fill in your local values"
    exit 1
fi

JOBS="${JOBS:-$(nproc)}"

if ! command -v cppcheck &>/dev/null; then
    echo "cppcheck not found"
    exit 1
fi

if [[ -z "${BOOST_SOURCE_ROOT:-}" || ! -d "$BOOST_SOURCE_ROOT" ]]; then
    echo "BOOST_SOURCE_ROOT is not set or does not exist"
    exit 1
fi

if [[ -d "$BOOST_SOURCE_ROOT/libs/regex/include/boost" ]]; then
    BOOST_REGEX_SRC_DIR="$BOOST_SOURCE_ROOT/libs/regex/include/boost"
else
    echo "BOOST_SOURCE_ROOT does not contain Boost.Regex source files"
    exit 1
fi

BOOST_REGEX_FILES=()
mapfile -t BOOST_REGEX_FILES < <(
    find "$BOOST_REGEX_SRC_DIR" \
        -type f \
        \( -name '*.h' -o -name '*.hpp' \) \
)

if [[ ${#BOOST_REGEX_FILES[@]} == 0 ]]; then
    echo "No source files found"
    exit 1
fi

mkdir -p "$REPORT_DIR"

cppcheck \
    -j "$JOBS" \
    --enable=all \
    --inconclusive \
    --language=c++ \
    --std=c++20 \
    --force \
    --platform=unix64 \
    --xml \
    --xml-version=2 \
    -DBOOST_REGEX_MODULE_EXPORT="" \
    -I"$BOOST_REGEX_SRC_DIR" "${BOOST_REGEX_FILES[@]}" \
    2>"$XML_FILE"

cppcheck-htmlreport \
    --file="$XML_FILE" \
    --report-dir="$REPORT_DIR/html" \
    --source-dir="$BOOST_REGEX_SRC_DIR"

xdg-open "$REPORT_DIR/html/index.html"