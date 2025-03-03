SCRIPT_DIR="$(dirname "$(readlink -f "$0")")"
MAIN_SO="$SCRIPT_DIR/main/obj-ia32/main.so"

if [ ! -f "$MAIN_SO" ]; then
  echo "Error: Cannot find $MAIN_SO. Please check its location."
  exit 1
fi

run_benchmark() {
  local dir="$1"
  local bench="$2"
  local fast_forward="$3"
  local orig_cmd="$4"

  echo "Starting benchmark '$bench' in directory '$dir'..."
  pushd "$dir" > /dev/null || { echo "Failed to change into $dir"; return 1; }

  mkdir -p results

  local modified_cmd
  modified_cmd=$(echo "$orig_cmd" | sed -E 's/(^|[[:space:]])>([[:space:]]*)([^ >]+)/\1> results\/\3/g; s/2>[[:space:]]*([^ >]+)/2> results\/\1/g')

  local full_cmd

  full_cmd="/usr/bin/time -o ${SCRIPT_DIR}/results/${bench}_time.txt pin -t \"$MAIN_SO\" -f \"$fast_forward\" -o $SCRIPT_DIR/results/${bench}.log -- $modified_cmd"

  touch ${SCRIPT_DIR}/results/${bench}_time.txt
  touch ${SCRIPT_DIR}/results/${bench}.log

  echo "Running command: $full_cmd"

  eval "$full_cmd"

  echo "Finished benchmark '$bench' in directory '$dir'."
  popd > /dev/null
}

benchmarks=(
  #"spec_2006/400.perlbench;perlbench;207;./perlbench_base.i386 -I./lib diffmail.pl 4 800 10 17 19 300 > perlbench.ref.diffmail.out 2> perlbench.ref.diffmail.err"
  #"spec_2006/401.bzip2;bzip2;301;./bzip2_base.i386 input.source 280 > bzip2.ref.source.out 2> bzip2.ref.source.err"
  #"spec_2006/403.gcc;gcc;107;./gcc_base.i386 cp-decl.i -o cp-decl.s > gcc.ref.cp-decl.out 2> gcc.ref.cp-decl.err"
  #"spec_2006/429.mcf;mcf;377;./mcf_base.i386 inp.in > mcf.ref.out 2> mcf.ref.err"
  "spec_2006/436.cactusADM;cactusADM;584;./cactusADM_base.i386 benchADM.par > cactusADM.ref.out 2> cactusADM.ref.err"
  "spec_2006/437.leslie3d;leslie3d;2346;./leslie3d_base.i386 < leslie3d.in > leslie3d.ref.out 2> leslie3d.ref.err"
  #"spec_2006/450.soplex;soplex;364;./soplex_base.i386 -m3500 ref.mps > soplex.ref.ref.out 2> soplex.ref.ref.err"
  #"spec_2006/456.hmmer;hmmer;264;./hmmer_base.i386 nph3.hmm swiss41 > hmmer.ref.nph3.out 2> hmmer.ref.nph3.err"
  "spec_2006/462.libquantum;libquantum;3605;./libquantum_base.i386 1397 8 > libquantum.ref.out 2> libquantum.ref.err"
  "spec_2006/470.lbm;lbm;830;./lbm_base.i386 3000 reference.dat 0 0 100_100_130_ldc.of > lbm.ref.out 2> lbm.ref.err"
  #"spec_2006/471.omnetpp;omnetpp;43;./omnetpp_base.i386 omnetpp.ini > omnetpp.ref.log 2> omnetpp.ref.err"
  "spec_2006/483.xalancbmk;xalancbmk;1331;./xalancbmk_base.i386 -v t5.xml xalanc.xsl > xalancbmk.ref.out 2> xalancbmk.ref.err"
)

pids=()

for entry in "${benchmarks[@]}"; do
  IFS=";" read -r dir bench fast_forward cmd <<< "$entry"
  run_benchmark "$dir" "$bench" "$fast_forward" "$cmd" &
  pids+=($!)
done

for pid in "${pids[@]}"; do
  wait "$pid"
done

echo "All benchmarks finished."

