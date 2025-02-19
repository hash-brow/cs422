#!/bin/bash
# Run all SPEC2006 benchmarks using PIN tool D.so

SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"
PIN_TOOL_DIR="$SCRIPT_DIR/main/obj-ia32"
PIN_SO="$PIN_TOOL_DIR/main.so"

SPEC_DIR="/home/noblepegasus/spec_2006"
RESULTS_DIR="$SCRIPT_DIR/results"

# Ensure results directory exists
mkdir -p "$RESULTS_DIR"

# Verify PIN tool exists
if [ ! -f "$PIN_SO" ]; then
  echo "❌ Error: Cannot find $PIN_SO"
  exit 1
fi

benchmarks=(
  "400.perlbench;perlbench;207;./perlbench_base.i386 -I./lib diffmail.pl 4 800 10 17 19 300"
  "401.bzip2;bzip2;301;./bzip2_base.i386 input.source 280"
  "403.gcc;gcc;107;./gcc_base.i386 cp-decl.i -o cp-decl.s"
  "429.mcf;mcf;377;./mcf_base.i386 inp.in"
  "450.soplex;soplex;364;./soplex_base.i386 -m3500 ref.mps"
  "456.hmmer;hmmer;264;./hmmer_base.i386 nph3.hmm swiss41"
  "471.omnetpp;omnetpp;43;./omnetpp_base.i386 omnetpp.ini"
)

pids=()

run_benchmark() {
  local dir="$1"
  local bench="$2"
  local fast_forward="$3"
  local orig_cmd="$4"

  echo "🚀 Starting benchmark '$bench'..."
  pushd "$SPEC_DIR/$dir" > /dev/null || { echo "❌ Failed to enter $dir"; return 1; }

  mkdir -p "$RESULTS_DIR/$bench"

  # Validate benchmark command
  if [[ -z "$orig_cmd" ]]; then
      echo "❌ Error: Benchmark command for '$bench' is empty!"
      popd > /dev/null
      return 1
  fi

  # Fix redirections using `:` as delimiter
  local modified_cmd
  modified_cmd=$(echo "$orig_cmd" | sed -E "s:(^|[[:space:]])>([[:space:]]*)([^ >]+):\1> $RESULTS_DIR/$bench/\3:g; s:2>[[:space:]]*([^ >]+):2> $RESULTS_DIR/$bench/\1:g")

  # Build full command (fixing the placement of `-o`)
  local full_cmd="pin -t \"$PIN_SO\" -f \"$fast_forward\" -o \"$RESULTS_DIR/$bench/isa_analysis.out\" -- $modified_cmd"

  echo "Executing: $full_cmd"
  eval "$full_cmd"

  echo "✅ Finished benchmark '$bench'. Results saved in $RESULTS_DIR/$bench/"
  popd > /dev/null
}

# Launch benchmarks in parallel
for entry in "${benchmarks[@]}"; do
  IFS=";" read -r dir bench fast_forward cmd <<< "$entry"
  run_benchmark "$dir" "$bench" "$fast_forward" "$cmd" &
  pids+=($!)
done

# Wait for all benchmarks
for pid in "${pids[@]}"; do
  wait "$pid"
done

echo "✅ All benchmarks completed. Results saved in $RESULTS_DIR/"
