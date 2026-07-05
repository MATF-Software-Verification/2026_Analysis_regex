#!/usr/bin/env bash
set -euo pipefail
 
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"
BINARY="$PROJECT_ROOT/build/regex_app"

if [[ ! -f "$BINARY" ]]; then 
    echo "regex_app not found"
    exit 1
fi

"$BINARY"