#!/usr/bin/env bash
set -euo pipefail

#### CONFIGURE HERE ############################################################
# Add or remove version names as needed:
VERSIONS=(v3a)
# Relative path from version root to your run.sh:
RUN_PATH="Ksim/cpus/sync/mips-fast/run.sh"
###############################################################################

ROOT_DIR="$(pwd)"

for ver in "${VERSIONS[@]}"; do
  VERSION_DIR="$ROOT_DIR/$ver"
  RUN_SCRIPT="$VERSION_DIR/$RUN_PATH"

  if [ ! -f "$RUN_SCRIPT" ]; then
    echo "Error: $RUN_SCRIPT not found." >&2
    exit 1
  fi

  echo "=== [$ver] Running run.sh ==="
  cd "$(dirname "$RUN_SCRIPT")"
  chmod +x "$(basename "$RUN_SCRIPT")"
  ./run.sh
  echo "=== [$ver] done ==="
  cd "$ROOT_DIR"
done

# Now run the verify script
VERIFY_SCRIPT="$ROOT_DIR/verify.sh"
if [ ! -f "$VERIFY_SCRIPT" ]; then
  echo "Error: $VERIFY_SCRIPT not found." >&2
  exit 1
fi

echo "=== Running verify.sh ==="
chmod +x "$VERIFY_SCRIPT"
./verify.sh

