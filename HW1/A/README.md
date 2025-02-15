# running stuff

Download and unzip `spec_2006` inside `cs422/HW1/A`.

```
$ cd main
$ ./cmp.sh
$ cd ..
$ ./run_spec_benchmarks.sh
```

For `make clean` do specify `TARGET=ia32`. Results of the benchmarks are in their respective spec CPU directories. For eg. `/cs422/HW1/A/spec_2006/400.perlbench/results` contains `perlbench_time.txt` which contains the time taken to run the benchmark, and `perlbench.log` contains the required output of the benchmark.

Ensure that `pin` is there in your shell `$PATH`. To modify which subset of the benchmarks are run, comment/uncomment lines in `run_spec_benchmark.sh`.

