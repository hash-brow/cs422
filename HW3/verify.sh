#!/usr/bin/env bash
set -euo pipefail

#### CONFIGURE HERE ###########################################################
# List all versions to compare; the first one is taken as the “baseline.”
# Just add more entries (e.g. v1, v3…) to this list when new versions appear.
VERSIONS=(v0 v1 v2 v3a v4 v5)
OUTPUT_ROOT="./outputs"
###############################################################################

# Baseline version to compare everything else against
BASE="${VERSIONS[0]}"

# Gather all the “*.out” files from the baseline
if [ ! -d "$OUTPUT_ROOT/$BASE" ]; then
  echo "Error: directory $OUTPUT_ROOT/$BASE does not exist." >&2
  exit 1
fi
FILES=$(cd "$OUTPUT_ROOT/$BASE" && ls *.out)

ALL_OK=1

for file in $FILES; do
  for ver in "${VERSIONS[@]:1}"; do
    BASE_F="$OUTPUT_ROOT/$BASE/$file"
    VER_F="$OUTPUT_ROOT/$ver/$file"

    if [ ! -f "$VER_F" ]; then
      echo " MISSING: $file in version $ver"
      ALL_OK=0
      continue
    fi

    # Compare, ignoring lines that begin with "Executing"
    if ! diff -q \
      <(grep -v '^Executing' "$BASE_F") \
      <(grep -v '^Executing' "$VER_F") \
      >/dev/null
    then
      echo " DIFFERENCE in $file between $BASE and $ver:"
      diff -u \
        <(grep -v '^Executing' "$BASE_F") \
        <(grep -v '^Executing' "$VER_F")
      ALL_OK=0
    fi
  done
done

if [ $ALL_OK -eq 1 ]; then
  echo "ALL OK"
fi

