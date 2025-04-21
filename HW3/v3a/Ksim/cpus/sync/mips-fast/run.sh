#!/usr/bin/env bash
set -euo pipefail

# 1) Compute ROOT_DIR as the absolute path of "../../../../../" from $PWD
ROOT_DIR="$(cd ../../../../../ && pwd)"

# 2) Compute VERSION_NUMBER = name of the directory at "../../../../"
VERSION_NUMBER="$(basename "$(cd ../../../../ && pwd)")"

# 4) Define RESULTS_DIR, OUTPUT_DIR, DEBUG_DIR, and make sure they exist
RESULTS_DIR="$ROOT_DIR/results/$VERSION_NUMBER"
OUTPUT_DIR="$ROOT_DIR/outputs/$VERSION_NUMBER"
DEBUG_DIR="$ROOT_DIR/debug/$VERSION_NUMBER"
mkdir -p "$RESULTS_DIR" "$OUTPUT_DIR" "$DEBUG_DIR"

# Clean & build

# 3) 5) 6) + debug copy
for test in c; do
  case "$test" in
    Subreg)     benchRel="Ksim/Bench/testcode/Subreg/subreg";;
    asm)        benchRel="Ksim/Bench/testcode/asm-sim/example";;
    c)          benchRel="Ksim/Bench/testcode/c-sim/example";;
    endian)     benchRel="Ksim/Bench/testcode/endian/endian";;
    factorial)  benchRel="Ksim/Bench/testcode/factorial/factorial";;
    fib)        benchRel="Ksim/Bench/testcode/fib/fib";;
    hello)      benchRel="Ksim/Bench/testcode/hello/hello";;
    host)       benchRel="Ksim/Bench/testcode/host/host";;
    ifactorial) benchRel="Ksim/Bench/testcode/ifactorial/ifactorial";;
    ifib)       benchRel="Ksim/Bench/testcode/ifib/ifib";;
    log2)       benchRel="Ksim/Bench/testcode/log2/log2";;
    msort)      benchRel="Ksim/Bench/testcode/msort/msort";;
    rfib)       benchRel="Ksim/Bench/testcode/rfib/rfib";;
    towers)     benchRel="Ksim/Bench/testcode/towers/towers";;
    vadd)       benchRel="Ksim/Bench/testcode/vadd/vadd";;
    *)
      echo "Unknown test: $test" >&2
      exit 1
      ;;
  esac

  FULL_BENCH="$ROOT_DIR/$VERSION_NUMBER/$benchRel"
  echo "=== Running $test on $FULL_BENCH ==="
  ./mipc "$FULL_BENCH" 

done

# Final cleanup


