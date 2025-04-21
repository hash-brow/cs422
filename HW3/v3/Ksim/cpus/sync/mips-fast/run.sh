#!/bin/bash

mkdir -p ~/v3/results/b/c
RESULT_DIR=~/v3/results/b/c

./mipc ~/v3/Ksim/Bench/testcode/Subreg/subreg
cp mipc.log $RESULT_DIR/subreg.log
./mipc ~/v3/Ksim/Bench/testcode/asm-sim/example
cp mipc.log $RESULT_DIR/asm.log
./mipc ~/v3/Ksim/Bench/testcode/c-sim/example
cp mipc.log $RESULT_DIR/c.log
./mipc ~/v3/Ksim/Bench/testcode/endian/endian
cp mipc.log $RESULT_DIR/endian.log
./mipc ~/v3/Ksim/Bench/testcode/factorial/factorial
cp mipc.log $RESULT_DIR/factorial.log
./mipc ~/v3/Ksim/Bench/testcode/fib/fib
cp mipc.log $RESULT_DIR/fib.log
./mipc ~/v3/Ksim/Bench/testcode/hello/hello
cp mipc.log $RESULT_DIR/hello.log
./mipc ~/v3/Ksim/Bench/testcode/host/host
cp mipc.log $RESULT_DIR/host.log
./mipc ~/v3/Ksim/Bench/testcode/ifactorial/ifactorial
cp mipc.log $RESULT_DIR/ifactorial.log
./mipc ~/v3/Ksim/Bench/testcode/ifib/ifib
cp mipc.log $RESULT_DIR/ifib.log
./mipc ~/v3/Ksim/Bench/testcode/log2/log2
cp mipc.log $RESULT_DIR/log2.log
./mipc ~/v3/Ksim/Bench/testcode/msort/msort
cp mipc.log $RESULT_DIR/msort.log
./mipc ~/v3/Ksim/Bench/testcode/rfib/rfib
cp mipc.log $RESULT_DIR/rfib.log
./mipc ~/v3/Ksim/Bench/testcode/towers/towers
cp mipc.log $RESULT_DIR/towers.log
./mipc ~/v3/Ksim/Bench/testcode/vadd/vadd
cp mipc.log $RESULT_DIR/vadd.log

