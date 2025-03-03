# running stuff

Download and unzip `spec_2006` inside `HW1`.

```
$ cd main
$ ./cmp.sh
$ cd ..
$ ./run_spec_benchmarks.sh
```

For `make clean` do specify `TARGET=ia32`. Results of the benchmarks are in `results/`. For eg. `perlbench_time.txt` contains the time taken to run the benchmark, and `perlbench.log` contains the required output of the benchmark.

Ensure that `pin` is there in your shell `$PATH`. To modify which subset of the benchmarks are run, comment/uncomment lines in `run_spec_benchmark.sh`.

Directory structure:

```
.
├── main
│   ├── cmp.sh
│   ├── main.cpp
│   ├── makefile
│   ├── makefile.rules
│   ├── paging.h
│   ├── utils.cpp
│   └── utils.h
├── README.md
├── report.pdf
├── results
│   ├── bzip2.log
│   ├── bzip2_time.txt
│   ├── cactusADM.log
│   ├── cactusADM_time.txt
│   ├── gcc.log
│   ├── gcc_time.txt
│   ├── hmmer.log
│   ├── hmmer_time.txt
│   ├── lbm.log
│   ├── lbm_time.txt
│   ├── leslie3d.log
│   ├── leslie3d_time.txt
│   ├── libquantum.log
│   ├── libquantum_time.txt
│   ├── mcf.log
│   ├── mcf_time.txt
│   ├── omnetpp.log
│   ├── omnetpp_time.txt
│   ├── perlbench.log
│   ├── perlbench_time.txt
│   ├── soplex.log
│   ├── soplex_time.txt
│   ├── sphinx3.log
│   ├── sphinx3_time.txt
│   ├── xalancbmk.log
│   └── xalancbmk_time.txt
└── run_spec_benchmarks.sh

2 directories, 36 files
```

