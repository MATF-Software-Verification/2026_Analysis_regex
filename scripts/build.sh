#!/usr/bin/env bash
set -euo pipefail
 
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"
CFG_FILE="$SCRIPT_DIR/project.cfg"
 
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
ENABLE_SANITIZERS="${ENABLE_SANITIZERS:-OFF}"
CXX="${CXX:-g++}"
JOBS="${JOBS:-$(nproc)}"

if [[ -z "${BOOST_ROOT:-}" || ! -d "$BOOST_ROOT" ]]; then
    echo "BOOST_ROOT is not set or does not exist"
    exit 1
fi

 
cmake -S "$PROJECT_ROOT" -B "$BUILD_DIR" \
    -DBOOST_ROOT="$BOOST_ROOT" \
    -DBoost_NO_SYSTEM_PATHS=ON \
    -DBoost_NO_BOOST_CMAKE=ON \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    -DCMAKE_CXX_COMPILER="$CXX" \
    -DENABLE_SANITIZERS="$ENABLE_SANITIZERS"
 
cmake --build "$BUILD_DIR" --parallel "$JOBS"