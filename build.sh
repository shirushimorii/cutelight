#!/usr/bin/env bash

set -u

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd -P)"
BUILD_TYPE="${1:-Release}"
BUILD_ROOT="$SCRIPT_DIR/builds"

if [[ ! -f "$SCRIPT_DIR/CMakeLists.txt" ]]; then
    echo "error: CMakeLists.txt not found in $SCRIPT_DIR" >&2
    exit 1
fi

if ! command -v cmake >/dev/null 2>&1; then
    echo "error: cmake is not installed" >&2
    exit 1
fi

mkdir -p "$BUILD_ROOT"

attempt=1
while [[ -e "$BUILD_ROOT/build$attempt" ]]; do
    attempt=$((attempt + 1))
done

BUILD_DIR="$BUILD_ROOT/build$attempt"
LOG_DIR="$BUILD_DIR/logs"
mkdir -p "$LOG_DIR" "$BUILD_DIR/runtime" "$BUILD_DIR/packaging"

if command -v ninja >/dev/null 2>&1; then
    GENERATOR="Ninja"
else
    GENERATOR="Unix Makefiles"
fi

LOG_FILE="$LOG_DIR/build.log"

{
    echo "Project: $SCRIPT_DIR"
    echo "Build attempt: $attempt"
    echo "Build type: $BUILD_TYPE"
    echo "Generator: $GENERATOR"
    echo "Build directory: $BUILD_DIR"
    echo "Started: $(date -u '+%Y-%m-%dT%H:%M:%SZ')"
    echo

    cmake -S "$SCRIPT_DIR" -B "$BUILD_DIR" -G "$GENERATOR" \
        -DCMAKE_BUILD_TYPE="$BUILD_TYPE" || exit $?
    cmake --build "$BUILD_DIR" --config "$BUILD_TYPE" --parallel || exit $?
} 2>&1 | tee "$LOG_FILE"

status=${PIPESTATUS[0]}
if [[ $status -ne 0 ]]; then
    echo "Build failed; logs and partial output preserved in: $BUILD_DIR" >&2
    exit "$status"
fi

cp -R "$SCRIPT_DIR/assets" "$BUILD_DIR/runtime/assets"
cp "$SCRIPT_DIR/packaging/com.mori.cutelight.desktop" "$BUILD_DIR/packaging/"
if [[ -f "$SCRIPT_DIR/LICENSE" ]]; then
    cp "$SCRIPT_DIR/LICENSE" "$BUILD_DIR/LICENSE"
fi

sed \
    -e "s|@BUILD_DIR@|$BUILD_DIR|g" \
    -e "s|@PROJECT_ROOT@|$SCRIPT_DIR|g" \
    "$SCRIPT_DIR/packaging/install.sh.in" > "$BUILD_DIR/install.sh"
chmod +x "$BUILD_DIR/install.sh"

{
    echo
    echo "Finished: $(date -u '+%Y-%m-%dT%H:%M:%SZ')"
    echo "Executable: $BUILD_DIR/cutelight"
    echo "Installer: $BUILD_DIR/install.sh"
} >> "$LOG_FILE"

echo "Build succeeded: $BUILD_DIR"
echo "Install with: cd '$BUILD_DIR' && ./install.sh"
