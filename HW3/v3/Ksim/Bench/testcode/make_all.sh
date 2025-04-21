#!/usr/bin/env bash
set -euo pipefail

for dir in */ ; do
  if [ -d "$dir" ]; then
    echo "=== Entering $dir ==="
    (cd "$dir" && gmake)
    echo "=== Done with $dir ==="
  fi
done

