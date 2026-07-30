#!/usr/bin/env bash
set -euo pipefail
 
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"
CFG_FILE="$PROJECT_ROOT/project.cfg"
 
if [[ -f "$CFG_FILE" ]]; then
    set -a
    source "$CFG_FILE"
    set +a
else
    echo "No project.cfg found. Copy the template and fill in your values:"
    echo "cp project.cfg.example project.cfg"
    exit 1
fi
 
BUILD_TYPE="${BUILD_TYPE:-Debug}"
ENABLE_ASAN="${ENABLE_ASAN:-ON}"
CXX="${CXX:-g++}"
JOBS="${JOBS:-$(nproc)}"

if [[ -z "${BOOST_ROOT:-}" ]]; then
    echo "BOOST_ROOT is not set in build.cfg"
    echo "This project requires a locally built Boost — set BOOST_ROOT in build.cfg"
    exit 1
fi
 
if [[ ! -d "$BOOST_ROOT" ]]; then
    echo "BOOST_ROOT directory does not exist: $BOOST_ROOT"
    echo "Build Boost first — see README for instructions."
    exit 1
fi
 
cmake -S "$PROJECT_ROOT" -B "$BUILD_DIR" \
    -DBOOST_ROOT="$BOOST_ROOT" \
    -DBoost_NO_SYSTEM_PATHS=ON \
    -DBoost_NO_BOOST_CMAKE=ON \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCMAKE_CXX_COMPILER="$CXX" \
    -DENABLE_ASAN="$ENABLE_ASAN"
 
cmake --build "$BUILD_DIR" --parallel "$JOBS"