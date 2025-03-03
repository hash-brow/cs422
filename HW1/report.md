# CS422: Computer Architecture
# Homework 1

## Implementation Details

Our implementation uses fast-forwarding at the basic block level, and we have also tried to incorporate the actual analysis at the basic block level to the maximum extent possible. 

To keep analysis calls slim, the calls simply increment the number of times a certain basic block is run, and almost all the relevant counting is done in the exit routine.

One aspect which cannot be dealt with this basic block counting method is the data memory footprint. As the memory operands can be dynamic, they have to be dealt with during analysis and also at the instruction-level.

Dumping all memory accesses and then processing them in the exit routine is sub-optimal as the dump will consume a lot of memory for any realistic application running for a long enough period of time. The other obvious option is to keep an array/bitset for all possible memory addresses. This is even more sub-optimal as we need a very big array/bitset, which would be completely impossible for 64 bit benchmarks if we had any.

To reduce the memory footprint of the pin tool, one would have to use some sort of a data structure to only log unique memory addresses. The obvious option would be a set, but it comes with a drawback - set insert calls at the time of analysis would be costly. 

The final solution we have implemented mimics the paging scheme of x86 using arrays - the idea is that we just want a bitset which can track which addresses we have touched, but large parts of the bitset will simply go unused. So, we implement a page directory/page table structure using arrays, and upon encountering an address corresponding to an unmapped page we allocate one by incrementing a few counters here and there. If there is not enough space to allocate a new page, we realloc() our arrays.

This scheme comes with the advantage of a low memory footprint, while being fast as well. If the page exists the analysis call is simply setting an array value to 1 after a few indirections. The OS and compiler will anyway keep the memory addresses limited to not too many pages, hence the costlier operations occur very rarely. 

The overall pin tool takes about 5-6 mins to run on perlbench.

The final results are reported on a server with 8 threads, and 32 GB RAM. The processor used is - AMD EPYC-Milan Processor

## Benchmark Results

### 400.perlbench diffmail.pl
#### Part A
| Instruction Type         | Absolute Value      | Percentage |
|--------------|-----------|------------|
| Instruction Count      | 1000000001 | -          |
| Loads         | 356838743 | 22.83%     |
| Stores        | 205970465 | 13.18%     |
| NOPs          | 959563    | 0.06%      |
| Direct Calls   | 12742485  | 0.82%      |
| Indirect Calls  | 2839560   | 0.18%      |
| Returns          | 15582046  | 1.00%      |
| Unconditional Branches    | 30549008  | 1.95%      |
| Conditional Branches      | 129984042 | 8.32%      |
| Logical operations      | 100106278 | 6.41%      |
| Rotate and Shift | 4275188   | 0.27%      |
| Flag Operations       | 862091    | 0.06%      |
| Vector Instructions       | 0         | 0.00%      |
| Conditional Moves         | 0         | 0.00%      |
| MMX and SSE instructions      | 0         | 0.00%      |
| System Calls      | 0         | 0.00%      |
| Floating Point Instructions          | 933010    | 0.06%      |
| The rest         | 701166731 | 44.87%     |

#### Part B
**CPI:** 25.84874

#### Part C
**Data Chunk Accesses:** 31101 \
**Instruction Chunk Accesses:** 2810

#### Part D
**Distribution of Instruction Length**
| Bytes | Count       |
|-------|------------|
| 0     | 0          |
| 1     | 117382740  |
| 2     | 256612066  |
| 3     | 274772730  |
| 4     | 52905377   |
| 5     | 78567185   |
| 6     | 185781333  |
| 7     | 33978530   |
| 8     | 28         |
| 9     | 0          |
| 10    | 12         |
| 11    | 0          |
| 12    | 0          |
| 13    | 0          |
| 14    | 0          |
| 15    | 0          |

**Distribution of the number of operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 959563     |
| 1        | 1072479    |
| 2        | 520827030  |
| 3        | 355062689  |
| 4        | 103215352  |
| 5        | 15583706   |
| 6        | 3279182    |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of register read operands in an instruction**
| Reads | Count       |
|-------|------------|
| 0     | 10082890   |
| 1     | 260353245  |
| 2     | 537426194  |
| 3     | 179162162  |
| 4     | 7097158    |
| 5     | 2599170    |
| 6     | 3279182    |
| 7     | 0          |
| 8     | 0          |
| 9     | 0          |

**Distribution of the number of register write operands in an instruction**
| Writes | Count       |
|--------|------------|
| 0      | 135800450  |
| 1      | 685177102  |
| 2      | 175739723  |
| 3      | 2394388    |
| 4      | 888338     |
| 5      | 0          |
| 6      | 0          |
| 7      | 0          |
| 8      | 0          |
| 9      | 0          |

**Distribution of the number of memory operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 453635188  |
| 1        | 530799022  |
| 2        | 15565791   |
| 3        | 0          |
| 4        | 0          |
| 5        | 0          |
| 6        | 0          |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory read operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 190659134  |
| 1        | 354817364  |
| 2        | 888315     |
| 3        | 0          |
| 4        | 0          |
| 5        | 0          |
| 6        | 0          |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory write operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 341028203  |
| 1        | 205336610  |
| 2        | 0          |
| 3        | 0          |
| 4        | 0          |
| 5        | 0          |
| 6        | 0          |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Maximum number of bytes touched by an instruction:** 8\
**Average number of bytes touched by an instruction:** 3.73807\
**Maximum value of immediate:** 2147483647\
**Minimum value of immediate:** -2147483648\
**Maximum value of displacement used in memory addressing:** 135918104\
**Minimum value of displacement used in memory addressing:** -1408

### 401.bzip2 input.source
#### Part A
| Instruction Type         | Absolute Value      | Percentage |
|--------------|-----------|------------|
| Instruction Count      | 1000000000 | -          |
| Loads         | 452710229 | 26.88%     |
| Stores        | 231178146 | 13.73%     |
| NOPs          | 36514    | 0.00%      |
| Direct Calls   | 791604  | 0.05%      |
| Indirect Calls  | 13   | 0.00%      |
| Returns          | 791614  | 0.05%      |
| Unconditional Branches    | 21299210  | 1.26%      |
| Conditional Branches      | 129923144 | 7.72%      |
| Logical operations      | 71000704 | 4.22%      |
| Rotate and Shift | 61831941   | 3.67%      |
| Flag Operations       | 6130    | 0.00%      |
| Vector Instructions       | 0         | 0.00%      |
| Conditional Moves         | 0         | 0.00%      |
| MMX and SSE instructions      | 0         | 0.00%      |
| System Calls      | 0         | 0.00%      |
| Floating Point Instructions          | 0    | 0.00%      |
| The rest         | 714319126 | 42.42%     |

#### Part B
**CPI:** 29.02341

#### Part C
**Data Chunk Accesses:** 2441019 \
**Instruction Chunk Accesses:** 754

#### Part D
**Distribution of Instruction Length**
| Bytes | Count       |
|-------|------------|
| 0     | 0          |
| 1     | 38611261  |
| 2     | 219201458  |
| 3     | 436965530  |
| 4     | 75327011   |
| 5     | 22047524   |
| 6     | 141357374  |
| 7     | 51341600   |
| 8     | 15085632   |
| 9     | 0          |
| 10    | 62610      |
| 11    | 0          |
| 12    | 0          |
| 13    | 0          |
| 14    | 0          |
| 15    | 0          |

**Distribution of the number of operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 36514      |
| 1        | 6147       |
| 2        | 597647448  |
| 3        | 382870411  |
| 4        | 2693869    |
| 5        | 14191403   |
| 6        | 2554208    |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of register read operands in an instruction**
| Reads | Count       |
|----------|------------|
| 0        | 5189068      |
| 1        | 183351652       |
| 2        | 533608125  |
| 3        | 215105844  |
| 4        | 46791304    |
| 5        | 13399799   |
| 6        | 2554208    |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of register write operands in an instruction**
| Writes | Count       |
|----------|------------|
| 0        | 132331400      |
| 1        | 712782483  |
| 2        | 152331892  |
| 3        | 2554225  |
| 4        | 0    |
| 5        | 0   |
| 6        | 0    |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 399856582      |
| 1        | 516406617       |
| 2        | 83736801  |
| 3        | 0  |
| 4        | 0    |
| 5        | 0   |
| 6        | 0    |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory read operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 147437265      |
| 1        | 452706153       |
| 2        | 0  |
| 3        | 0  |
| 4        | 0    |
| 5        | 0   |
| 6        | 0    |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory write operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 368969352      |
| 1        | 231174066       |
| 2        | 0 |
| 3        | 0  |
| 4        | 0    |
| 5        | 0   |
| 6        | 0    |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Maximum number of bytes touched by an instruction:** 8 \
**Average number of bytes touched by an instruction:** 3.48541 \
**Maximum value of immediate:** 1431655766 \
**Minimum value of immediate:** -858993459 \
**Maximum value of displacement used in memory addressing:** 135000192 \
**Minimum value of displacement used in memory addressing:** -4848

### 403.gcc cp-decl.i
#### Part A
| Instruction Type         | Absolute Value      | Percentage |
|--------------|-----------|------------|
| Instruction Count      | 999999998 | -          |
| Loads         | 138311348 | 9.24%     |
| Stores        | 359267344 | 23.99%     |
| NOPs          | 189282    | 0.01%      |
| Direct Calls   | 4590551  | 0.31%      |
| Indirect Calls  | 501863   | 0.03%      |
| Returns          | 5092413  | 0.34%      |
| Unconditional Branches    | 4934339  | 0.33%      |
| Conditional Branches      | 133478830 | 8.91%      |
| Logical operations      | 131969278 | 8.81%      |
| Rotate and Shift | 2349699   | 0.16%      |
| Flag Operations       | 184910    | 0.01%      |
| Vector Instructions       | 0         | 0.00%      |
| Conditional Moves         | 0         | 0.00%      |
| MMX and SSE instructions      | 0         | 0.00%      |
| System Calls      | 0         | 0.00%      |
| Floating Point Instructions          | 5    | 0.00%      |
| The rest         | 716708828 | 47.86%     |

#### Part B
**CPI:** 23.92563

#### Part C
**Data Chunk Accesses:** 1140752 \
**Instruction Chunk Accesses:** 2953

#### Part D
**Distribution of Instruction Length**
| Bytes | Count       |
|-------|------------|
| 0     | 0          |
| 1     | 129831292  |
| 2     | 591656217  |
| 3     | 124866097  |
| 4     | 115982272  |
| 5     | 11208076   |
| 6     | 15635333   |
| 7     | 10762372   |
| 8     | 58339      |
| 9     | 0          |
| 10    | 0          |
| 11    | 0          |
| 12    | 0          |
| 13    | 0          |
| 14    | 0          |
| 15    | 0          |

**Distribution of the number of operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 189282      |
| 1        | 4598587       |
| 2        | 349464457  |
| 3        | 403930536  |
| 4        | 33847780   |
| 5        | 203714073   |
| 6        | 4255283    |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of register read operands in an instruction**
| Reads | Count       |
|----------|------------|
| 0        | 2136960      |
| 1        | 168992759       |
| 2        | 472424311  |
| 3        | 62601062  |
| 4        | 90467499    |
| 5        | 199122124   |
| 6        | 4255283    |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of register write operands in an instruction**
| Writes | Count       |
|----------|------------|
| 0        | 125743996     |
| 1        | 418779303       |
| 2        | 451078536  |
| 3        | 4398163 |
| 4        | 0   |
| 5        | 0   |
| 6        | 0    |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |


**Distribution of the number of memory operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 518888091      |
| 1        | 464713530       |
| 2        | 16398377  |
| 3        | 0  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |


**Distribution of the number of memory read operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 342800562      |
| 1        | 138311345       |
| 2        | 0  |
| 3        | 0  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0    |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory write operands in an instruction**
| Opernads | Count       |
|----------|------------|
| 0        | 121912968      |
| 1        | 359198939      |
| 2        | 0  |
| 3        | 0  |
| 4        | 0  |
| 5        | 0  |
| 6        | 0    |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Maximum number of bytes touched by an instruction:** 8 \
**Average number of bytes touched by an instruction:** 3.95864 \
**Maximum value of immediate:** 1073741823 \
**Minimum value of immediate:** -2147483587 \
**Maximum value of displacement used in memory addressing:** 138634432 \
**Minimum value of displacement used in memory addressing:** -1744

### 429.mcf
#### Part A
| Instruction Type         | Absolute Value      | Percentage |
|--------------|-----------|------------|
| Instruction Count      | 999999995 | -          |
| Loads         | 415212714 | 27.22%     |
| Stores        | 110040152 | 7.21%     |
| NOPs          | 1477639    | 0.10%      |
| Direct Calls   | 12556301  | 0.82%      |
| Indirect Calls  | 0   | 0.00%      |
| Returns          | 12556300  | 0.82%      |
| Unconditional Branches    | 8314496  | 0.55%      |
| Conditional Branches      | 178243015 | 11.69%      |
| Logical operations      | 75119495 | 4.93%      |
| Rotate and Shift | 3516416   | 0.23%      |
| Flag Operations       | 0    | 0.00%      |
| Vector Instructions       | 0         | 0.00%      |
| Conditional Moves         | 0         | 0.00%      |
| MMX and SSE instructions      | 0         | 0.00%      |
| System Calls      | 0         | 0.00%      |
| Floating Point Instructions          | 0    | 0.00%      |
| The rest         | 708216333 | 46.43%     |

#### Part B
**CPI:** 24.76160

#### Part C
**Data Chunk Accesses:** 11672843 \
**Instruction Chunk Accesses:** 66

#### Part D
**Distribution of Instruction Length**
| Bytes | Count       |
|-------|------------|
| 0     | 0          |
| 1     | 80626429   |
| 2     | 485393436  |
| 3     | 315526342  |
| 4     | 50531205   |
| 5     | 22076550   |
| 6     | 5249820    |
| 7     | 40536213   |
| 8     | 0          |
| 9     | 0          |
| 10    | 0          |
| 11    | 0          |
| 12    | 0          |
| 13    | 0          |
| 14    | 0          |
| 15    | 0          |

**Distribution of the number of operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 1477639      |
| 1        | 0       |
| 2        | 484784798  |
| 3        | 457347095  |
| 4        | 43834162   |
| 5        | 12556301   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of register read operands in an instruction**
| Reads | Count       |
|----------|------------|
| 0        | 3759879      |
| 1        | 148510619       |
| 2        | 677530557  |
| 3        | 170198940  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of register write operands in an instruction**
| Writes | Count       |
|----------|------------|
| 0        | 70906605      |
| 1        | 770283672       |
| 2        | 158771206  |
| 3        | 38512  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 487182329      |
| 1        | 500382455       |
| 2        | 12435211  |
| 3        | 0  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory read operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 97604946      |
| 1        | 415212720       |
| 2        | 0  |
| 3        | 0  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory write operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 402777509      |
| 1        | 110040157       |
| 2        | 0  |
| 3        | 0  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Maximum number of bytes touched by an instruction:** 4 \
**Average number of bytes touched by an instruction:** 4.00 \
**Maximum value of immediate:** 1374389535 \
**Minimum value of immediate:** -100000000 \
**Maximum value of displacement used in memory addressing:** 134957120 \
**Minimum value of displacement used in memory addressing:** -76 

### 450.soplex ref.mps

#### Part A
| Instruction Type         | Absolute Value      | Percentage |
|--------------|-----------|------------|
| Instruction Count      | 999999995 | -          |
| Loads         | 547816765 | 33.27%     |
| Stores        | 98782934 | 6.00%     |
| NOPs          | 3550    | 0.00%      |
| Direct Calls   | 3177232  | 0.19%      |
| Indirect Calls  | 111   | 0.00%      |
| Returns          | 3177343  | 0.19%      |
| Unconditional Branches    | 12754267  | 0.77%      |
| Conditional Branches      | 103254427 | 6.27%      |
| Logical operations      | 13997730 | 0.85%      |
| Rotate and Shift | 10484108   | 0.64%      |
| Flag Operations       | 22967932    | 1.39%      |
| Vector Instructions       | 0         | 0.00%      |
| Conditional Moves         | 0         | 0.00%      |
| MMX and SSE instructions      | 0         | 0.00%      |
| System Calls      | 0         | 0.00%      |
| Floating Point Instructions          | 309501735    | 18.80%      |
| The rest         | 520681560 | 31.62%     |

#### Part B
**CPI:** 28.09546

#### Part C
**Data Chunk Accesses:** 5658598 \
**Instruction Chunk Accesses:** 646

#### Part D
**Distribution of Instruction Length**
| Bytes | Count       |
|-------|------------|
| 0     | 0          |
| 1     | 77591041   |
| 2     | 441944595  |
| 3     | 399698003  |
| 4     | 16087417   |
| 5     | 3219650    |
| 6     | 40002789   |
| 7     | 17519588   |
| 8     | 3936912    |
| 9     | 0          |
| 10    | 0          |
| 11    | 0          |
| 12    | 0          |
| 13    | 0          |
| 14    | 0          |
| 15    | 0          |

**Distribution of the number of operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 3550      |
| 1        | 13       |
| 2        | 412391793  |
| 3        | 396045259  |
| 4        | 188350993   |
| 5        | 3208387   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of register read operands in an instruction**
| Reads | Count       |
|----------|------------|
| 0        | 23142637      |
| 1        | 216228422       |
| 2        | 578652863  |
| 3        | 137989682  |
| 4        | 43955254   |
| 5        | 31137   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of register write operands in an instruction**
| Writes | Count       |
|----------|------------|
| 0        | 68172486      |
| 1        | 765092947       |
| 2        | 166734562  |
| 3        | 0  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 531479062      |
| 1        | 439276467       |
| 2        | 29244466  |
| 3        | 0  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory read operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 51702071      |
| 1        | 416818862       |
| 2        | 0  |
| 3        | 0  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory write operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 387574396      |
| 1        | 80946537       |
| 2        | 0  |
| 3        | 0  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Maximum number of bytes touched by an instruction:** 8 \
**Average number of bytes touched by an instruction:** 5.27066 \
**Maximum value of immediate:** 2147483647 \
**Minimum value of immediate:** -640172613 \
**Maximum value of displacement used in memory addressing:** 135855532 \
**Minimum value of displacement used in memory addressing:** -344 

### 456.hmmer nph3.hmm
#### Part A
| Instruction Type         | Absolute Value      | Percentage |
|--------------|-----------|------------|
| Instruction Count      | 1000000001 | -          |
| Loads         | 547671722 | 33.74%     |
| Stores        | 75699710 | 4.66%     |
| NOPs          | 34317    | 0.00%      |
| Direct Calls   | 144622  | 0.01%      |
| Indirect Calls  | 959   | 0.00%      |
| Returns          | 145581  | 0.01%      |
| Unconditional Branches    | 205862  | 0.01%      |
| Conditional Branches      | 144361425 | 8.89%      |
| Logical operations      | 1158703 | 0.07%      |
| Rotate and Shift | 294106   | 0.02%      |
| Flag Operations       | 5669    | 0.00%      |
| Vector Instructions       | 0         | 0.00%      |
| Conditional Moves         | 0         | 0.00%      |
| MMX and SSE instructions      | 0         | 0.00%      |
| System Calls      | 0         | 0.00%      |
| Floating Point Instructions          | 40212    | 0.00%      |
| The rest         | 853608545 | 52.58%     |

#### Part B
**CPI:** 27.49586

#### Part C
**Data Chunk Accesses:** 84052 \
**Instruction Chunk Accesses:** 486

#### Part D
**Distribution of Instruction Length**
| Bytes | Count       |
|-------|------------|
| 0     | 0          |
| 1     | 25078459   |
| 2     | 302860552  |
| 3     | 296150918  |
| 4     | 270388796  |
| 5     | 24861234   |
| 6     | 68360207   |
| 7     | 416480     |
| 8     | 11883355   |
| 9     | 0          |
| 10    | 0          |
| 11    | 0          |
| 12    | 0          |
| 13    | 0          |
| 14    | 0          |
| 15    | 0          |

**Distribution of the number of operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 34887      |
| 1        | 3512       |
| 2        | 566126763  |
| 3        | 432938365  |
| 4        | 713115   |
| 5        | 159551   |
| 6        | 23808   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of register read operands in an instruction**
| Reads | Count       |
|----------|------------|
| 0        | 610947      |
| 1        | 75620185       |
| 2        | 562398800  |
| 3        | 281425794  |
| 4        | 79905538   |
| 5        | 14929   |
| 6        | 23808   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of register write operands in an instruction**
| Writes | Count       |
|----------|------------|
| 0        | 75149179      |
| 1        | 755279998       |
| 2        | 169547016  |
| 3        | 23808  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 376809117      |
| 1        | 623024775       |
| 2        | 166109  |
| 3        | 0  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory read operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 75528003      |
| 1        | 547662881       |
| 2        | 0  |
| 3        | 0  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory write operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 547496772      |
| 1        | 75694112       |
| 2        | 0  |
| 3        | 0  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Maximum number of bytes touched by an instruction:** 8 \
**Average number of bytes touched by an instruction:** 3.99751 \
**Maximum value of immediate:** 2147483647 \
**Minimum value of immediate:** -987654321 \
**Maximum value of displacement used in memory addressing:** 135294312 \
**Minimum value of displacement used in memory addressing:** -580

### 471.omnetpp
#### Part A
| Instruction Type         | Absolute Value      | Percentage |
|--------------|-----------|------------|
| Instruction Count      | 1000000004 | -          |
| Loads         | 371235139 | 23.19%     |
| Stores        | 229791102 | 14.35%     |
| NOPs          | 802403    | 0.05%      |
| Direct Calls   | 21327723  | 1.33%      |
| Indirect Calls  | 3689258   | 0.23%      |
| Returns          | 25016982  | 1.56%      |
| Unconditional Branches    | 22189666  | 1.39%      |
| Conditional Branches      | 117335247 | 7.33%      |
| Logical operations      | 60009446 | 3.75%      |
| Rotate and Shift | 7139686   | 0.45%      |
| Flag Operations       | 20159872    | 1.26%      |
| Vector Instructions       | 0         | 0.00%      |
| Conditional Moves         | 0         | 0.00%      |
| MMX and SSE instructions      | 0         | 0.00%      |
| System Calls      | 0         | 0.00%      |
| Floating Point Instructions          | 96963587    | 6.06%      |
| The rest         | 625366134 | 39.06%     |

#### Part B
**CPI:** 26.90264

#### Part C
**Data Chunk Accesses:** 528736 \
**Instruction Chunk Accesses:** 891

#### Part D
**Distribution of Instruction Length**
| Bytes | Count       |
|-------|------------|
| 0     | 0          |
| 1     | 154578800  |
| 2     | 308551256  |
| 3     | 382074579  |
| 4     | 34347873   |
| 5     | 45117268   |
| 6     | 48822851   |
| 7     | 26506956   |
| 8     | 0          |
| 9     | 0          |
| 10    | 421        |
| 11    | 0          |
| 12    | 0          |
| 13    | 0          |
| 14    | 0          |
| 15    | 0          |

**Distribution of the number of operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 802403      |
| 1        | 225879       |
| 2        | 518294369  |
| 3        | 281743910  |
| 4        | 172763956   |
| 5        | 25030572   |
| 6        | 1138915   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of register read operands in an instruction**
| Reads | Count       |
|----------|------------|
| 0        | 29000202     |
| 1        | 197518845       |
| 2        | 540431949  |
| 3        | 219765979  |
| 4        | 8457503   |
| 5        | 3686611   |
| 6        | 1138915   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of register write operands in an instruction**
| Writes | Count       |
|----------|------------|
| 0        | 165349972      |
| 1        | 665542439       |
| 2        | 167592664  |
| 3        | 376014  |
| 4        | 1138915   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 449250115      |
| 1        | 542984766      |
| 2        | 7765123  |
| 3        | 0  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory read operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 212450802      |
| 1        | 337160172       |
| 2        | 1138915  |
| 3        | 0  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory write operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 331672879      |
| 1        | 219077010       |
| 2        | 0  |
| 3        | 0  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Maximum number of bytes touched by an instruction:** 8 \
**Average number of bytes touched by an instruction:** 4.22175 \
**Maximum value of immediate:** 2147483647 \
**Minimum value of immediate:** -2092037281 \
**Maximum value of displacement used in memory addressing:** 136090116 \
**Minimum value of displacement used in memory addressing:** -104

### 483.xalancbmk
#### Part A
| Instruction Type         | Absolute Value      | Percentage |
|--------------|-----------|------------|
| Instruction Count      | 999999999 | -          |
| Loads         | 368285554 | 23.95%     |
| Stores        | 169743280 | 11.04%     |
| NOPs          | 22004073    | 1.43%      |
| Direct Calls   | 13270384  | 0.86%      |
| Indirect Calls  | 8947553   | 0.58%      |
| Returns          | 22216327  | 1.44%      |
| Unconditional Branches    | 8619037  | 0.56%      |
| Conditional Branches      | 177692163 | 11.55%      |
| Logical operations      | 37949205 | 2.47%      |
| Rotate and Shift | 5621502   | 0.37%      |
| Flag Operations       | 1675102 | 0.11%      |
| Vector Instructions       | 0         | 0.00%      |
| Conditional Moves         | 0         | 0.00%      |
| MMX and SSE instructions      | 0         | 0.00%      |
| System Calls      | 0         | 0.00%      |
| Floating Point Instructions          | 7312742    | 0.48%      |
| The rest         | 694691911 | 45.17%     |

#### Part B
**CPI:** 25.13738

#### Part C
**Data Chunk Accesses:** 838186 \
**Instruction Chunk Accesses:** 2299

#### Part D
**Distribution of Instruction Length**
| Bytes | Count       |
|-------|------------|
| 0     | 0          |
| 1     | 145417397  |
| 2     | 320264376  |
| 3     | 446452310  |
| 4     | 31889277   |
| 5     | 24145344   |
| 6     | 23732563   |
| 7     | 7709324    |
| 8     | 262554     |
| 9     | 60557      |
| 10    | 66297      |
| 11    | 0          |
| 12    | 0          |
| 13    | 0          |
| 14    | 0          |
| 15    | 0          |

**Distribution of the number of operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 22004073      |
| 1        | 748204       |
| 2        | 423137418  |
| 3        | 412389160  |
| 4        | 102000945   |
| 5        | 24177375   |
| 6        | 15542824   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of register read operands in an instruction**
| Reads | Count       |
|----------|------------|
| 0        | 29470106      |
| 1        | 236655527       |
| 2        | 457874007  |
| 3        | 250423600  |
| 4        | 1205380   |
| 5        | 8828555   |
| 6        | 15542824   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of register write operands in an instruction**
| Writes | Count       |
|----------|------------|
| 0        | 113551163      |
| 1        | 685190722      |
| 2        | 185713783  |
| 3        | 15544331  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 493479671      |
| 1        | 479447201       |
| 2        | 27073127  |
| 3        | 0  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory read operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 141051745      |
| 1        | 365468583       |
| 2        | 0  |
| 3        | 0  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory write operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 338395456      |
| 1        | 168124872       |
| 2        | 0  |
| 3        | 0  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Maximum number of bytes touched by an instruction:** 8 \
**Average number of bytes touched by an instruction:** 4.15831 \
**Maximum value of immediate:** 2147483647 \
**Minimum value of immediate:** -1431655765 \
**Maximum value of displacement used in memory addressing:** 139657912 \
**Minimum value of displacement used in memory addressing:** -1392 

## Optional Benchmarks

### 436.cactusADM
#### Part A
| Instruction Type         | Absolute Value      | Percentage |
|--------------|-----------|------------|
| Instruction Count      | 1000000025 | -          |
| Loads         | 1040837004 | 43.88%     |
| Stores        | 331288182 | 13.97%     |
| NOPs          | 2265  | 0.00%      |
| Direct Calls   | 531584  | 0.02%      |
| Indirect Calls  | 193   | 0.00%      |
| Returns          | 531777  | 0.02%      |
| Unconditional Branches    | 535271  | 0.02%      |
| Conditional Branches      | 4305620 | 0.18%      |
| Logical operations      | 1626453 | 0.07%      |
| Rotate and Shift | 1059310   | 0.04%      |
| Flag Operations       | 5    | 0.00%      |
| Vector Instructions       | 0         | 0.00%      |
| Conditional Moves         | 0         | 0.00%      |
| MMX and SSE instructions      | 0         | 0.00%      |
| System Calls      | 0         | 0.00%      |
| Floating Point Instructions          | 851723546    | 35.90%      |
| The rest         | 139684001 | 5.89%     |

#### Part B
**CPI:** 40.91289

#### Part C
**Data Chunk Accesses:** 4378243 \
**Instruction Chunk Accesses:** 1225

#### Part D
**Distribution of Instruction Length**
| Bytes | Count       |
|-------|------------|
| 0     | 0          |
| 1     | 1658078    |
| 2     | 263441920  |
| 3     | 78112037   |
| 4     | 15678306   |
| 5     | 1609528    |
| 6     | 603728417  |
| 7     | 35771739   |
| 8     | 0          |
| 9     | 0          |
| 10    | 0          |
| 11    | 0          |
| 12    | 0          |
| 13    | 0          |
| 14    | 0          |
| 15    | 0          |

**Distribution of the number of operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 2265      |
| 1        | 23       |
| 2        | 83585145  |
| 3        | 461382409  |
| 4        | 454498376   |
| 5        | 531777   |
| 6        | 30   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of register read operands in an instruction**
| Reads | Count       |
|----------|------------|
| 0        | 182747783      |
| 1        | 42914499       |
| 2        | 532137882  |
| 3        | 397790818  |
| 4        | 8881878   |
| 5        | 135   |
| 6        | 30   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of register write operands in an instruction**
| Writes | Count       |
|----------|------------|
| 0        | 149849538      |
| 1        | 791740403      |
| 2        | 58409928  |
| 3        | 126  |
| 4        | 30   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 273540897      |
| 1        | 690600411       |
| 2        | 35858717  |
| 3        | 0  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory read operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 150924591      |
| 1        | 575534507       |
| 2        | 30  |
| 3        | 0  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory write operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 539675850      |
| 1        | 186783278       |
| 2        | 0  |
| 3        | 0  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Maximum number of bytes touched by an instruction:** 8 \
**Average number of bytes touched by an instruction:** 7.35779 \
**Maximum value of immediate:** 1431655766 \
**Minimum value of immediate:** -2147483648 \
**Maximum value of displacement used in memory addressing:** 135701372 \
**Minimum value of displacement used in memory addressing:** -2620

### 437.leslie3d
#### Part A
| Instruction Type         | Absolute Value      | Percentage |
|--------------|-----------|------------|
| Instruction Count      | 999999974 | -          |
| Loads         | 736425922 | 38.01%     |
| Stores        | 201007390 | 10.37%     |
| NOPs          | 83321    | 0.00%      |
| Direct Calls   | 913  | 0.00%      |
| Indirect Calls  | 17   | 0.00%      |
| Returns          | 930  | 0.00%      |
| Unconditional Branches    | 169181  | 0.01%      |
| Conditional Branches      | 41549285 | 2.14%      |
| Logical operations      | 435798 | 0.02%      |
| Rotate and Shift | 5168464   | 0.27%      |
| Flag Operations       | 32    | 0.00%      |
| Vector Instructions       | 0         | 0.00%      |
| Conditional Moves         | 0         | 0.00%      |
| MMX and SSE instructions      | 0         | 0.00%      |
| System Calls      | 0         | 0.00%      |
| Floating Point Instructions          | 394808052    | 20.38%      |
| The rest         | 557783981 | 28.79%     |

#### Part B
**CPI:** 34.38587

#### Part C
**Data Chunk Accesses:** 2455850 \
**Instruction Chunk Accesses:** 2632

#### Part D
**Distribution of Instruction Length**
| Bytes | Count       |
|-------|------------|
| 0     | 0          |
| 1     | 34716289   |
| 2     | 503707191  |
| 3     | 58913112   |
| 4     | 82555      |
| 5     | 412264     |
| 6     | 400346767  |
| 7     | 1821778    |
| 8     | 0          |
| 9     | 0          |
| 10    | 18         |
| 11    | 0          |
| 12    | 0          |
| 13    | 0          |
| 14    | 0          |
| 15    | 0          |

**Distribution of the number of operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 83321      |
| 1        | 33       |
| 2        | 317860956  |
| 3        | 492867782  |
| 4        | 189186846   |
| 5        | 992   |
| 6        | 44   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of register read operands in an instruction**
| Reads | Count       |
|----------|------------|
| 0        | 357048      |
| 1        | 58705861       |
| 2        | 653895054  |
| 3        | 270599963  |
| 4        | 16441932   |
| 5        | 72   |
| 6        | 44   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of register write operands in an instruction**
| Writes | Count       |
|----------|------------|
| 0        | 68909634      |
| 1        | 735959862       |
| 2        | 195130320  |
| 3        | 158  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 378021898      |
| 1        | 548828586       |
| 2        | 73149490  |
| 3        | 0  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory read operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 68829595      |
| 1        | 553148481       |
| 2        | 0  |
| 3        | 0  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory write operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 479998991      |
| 1        | 141979085       |
| 2        | 0  |
| 3        | 0  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Maximum number of bytes touched by an instruction:** 10 \
**Average number of bytes touched by an instruction:** 5.55829 \
**Maximum value of immediate:** 2147483647 \
**Minimum value of immediate:** -2147483648 \
**Maximum value of displacement used in memory addressing:** 135182404 \
**Minimum value of displacement used in memory addressing:** -1760

### 462.libquantum
#### Part A
| Instruction Type         | Absolute Value      | Percentage |
|--------------|-----------|------------|
| Instruction Count      | 1000000012 | -          |
| Loads         | 279866875 | 19.86%     |
| Stores        | 129203343 | 9.17%     |
| NOPs          | 0    | 0.00%      |
| Direct Calls   | 556664  | 0.04%      |
| Indirect Calls  | 0   | 0.00%      |
| Returns          | 5556665  | 0.04%      |
| Unconditional Branches    | 834555  | 0.06%      |
| Conditional Branches      | 157417367 | 11.17%      |
| Logical operations      | 146293204 | 10.38%      |
| Rotate and Shift | 107083358   | 7.60%      |
| Flag Operations       | 0    | 0.00%      |
| Vector Instructions       | 0         | 0.00%      |
| Conditional Moves         | 0         | 0.00%      |
| MMX and SSE instructions      | 0         | 0.00%      |
| System Calls      | 0         | 0.00%      |
| Floating Point Instructions          | 0    | 0.00%      |
| The rest         | 587258199 | 41.68%     |

#### Part B
**CPI:** 21.03154

#### Part C
**Data Chunk Accesses:** 1048592 \
**Instruction Chunk Accesses:** 68

#### Part D
**Distribution of Instruction Length**
| Bytes | Count       |
|-------|------------|
| 0     | 0          |
| 1     | 61187360   |
| 2     | 442809998  |
| 3     | 437560012  |
| 4     | 51733083   |
| 5     | 834758     |
| 6     | 1113146    |
| 7     | 4761655    |
| 8     | 0          |
| 9     | 0          |
| 10    | 0          |
| 11    | 0          |
| 12    | 0          |
| 13    | 0          |
| 14    | 0          |
| 15    | 0          |

**Distribution of the number of operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 0      |
| 1        | 0      |
| 2        | 467945159  |
| 3        | 476703754  |
| 4        | 54794435   |
| 5        | 556664   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of register read operands in an instruction**
| Reads | Count       |
|----------|------------|
| 0        | 0      |
| 1        | 314502858       |
| 2        | 461095420  |
| 3        | 223845200  |
| 4        | 556534   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of register write operands in an instruction**
| Writes | Count       |
|----------|------------|
| 0        | 101552907      |
| 1        | 655482274       |
| 2        | 242408553  |
| 3        | 556278  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 616075341      |
| 1        | 358779120       |
| 2        | 25145551  |
| 3        | 0  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory read operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 104057794      |
| 1        | 279866877       |
| 2        | 0  |
| 3        | 0  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory write operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 254721326      |
| 1        | 129203345       |
| 2        | 0  |
| 3        | 0  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Maximum number of bytes touched by an instruction:** 4 \
**Average number of bytes touched by an instruction:** 3.60445 \
**Maximum value of immediate:** 124 \
**Minimum value of immediate:** -1 \
**Maximum value of displacement used in memory addressing:** 134982404 \
**Minimum value of displacement used in memory addressing:** -64

### 470.lbm
#### Part A
| Instruction Type         | Absolute Value      | Percentage |
|--------------|-----------|------------|
| Instruction Count      | 999999999 | -          |
| Loads         | 709947171 | 35.99%     |
| Stores        | 262836303 | 13.32%     |
| NOPs          | 14    | 0.00%      |
| Direct Calls   | 163  | 0.00%      |
| Indirect Calls  | 16   | 0.00%      |
| Returns          | 179  | 0.00%      |
| Unconditional Branches    | 4767427  | 0.24%      |
| Conditional Branches      | 7791725 | 0.39%      |
| Logical operations      | 5059493 | 0.26%      |
| Rotate and Shift | 196   | 0.00%      |
| Flag Operations       | 79396    | 0.00%      |
| Vector Instructions       | 0         | 0.00%      |
| Conditional Moves         | 0         | 0.00%      |
| MMX and SSE instructions      | 0         | 0.00%      |
| System Calls      | 0         | 0.00%      |
| Floating Point Instructions          | 965338837    | 48.93%      |
| The rest         | 16962553 | 0.86%     |

#### Part B
**CPI:** 35.02404 

#### Part C
**Data Chunk Accesses:** 13121299 \
**Instruction Chunk Accesses:** 381 

#### Part D
**Distribution of Instruction Length**
| Bytes | Count       |
|-------|------------|
| 0     | 0          |
| 1     | 106686     |
| 2     | 487544911  |
| 3     | 179628039  |
| 4     | 891        |
| 5     | 7579131    |
| 6     | 324469699  |
| 7     | 645484     |
| 8     | 0          |
| 9     | 0          |
| 10    | 25158      |
| 11    | 0          |
| 12    | 0          |
| 13    | 0          |
| 14    | 0          |
| 15    | 0          |

**Distribution of the number of operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 14      |
| 1        | 19       |
| 2        | 13365172  |
| 3        | 457059385  |
| 4        | 529575202   |
| 5        | 179   |
| 6        | 28   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of register read operands in an instruction**
| Reads | Count       |
|----------|------------|
| 0        | 44824719      |
| 1        | 133972972       |
| 2        | 604379423  |
| 3        | 216822702  |
| 4        | 139   |
| 5        | 16   |
| 6        | 28   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of register write operands in an instruction**
| Writes | Count       |
|----------|------------|
| 0        | 131900105      |
| 1        | 822007170       |
| 2        | 46092569  |
| 3        | 155  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 510666322     |
| 1        | 489308326       |
| 2        | 25351  |
| 3        | 0  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory read operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 131821462      |
| 1        | 357512215       |
| 2        | 0  |
| 3        | 0  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory write operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 357486864      |
| 1        | 131846813       |
| 2        | 0  |
| 3        | 0  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Maximum number of bytes touched by an instruction:** 8 \
**Average number of bytes touched by an instruction:** 7.95169 \
**Maximum value of immediate:** 2147483647 \
**Minimum value of immediate:** -1672357186 \
**Maximum value of displacement used in memory addressing:** 3216104 \
**Minimum value of displacement used in memory addressing:** -16080

### 482.sphinx3
#### Part A
| Instruction Type         | Absolute Value      | Percentage |
|--------------|-----------|------------|
| Instruction Count      | 999999997 | -          |
| Loads         | 441157275 | 29.21%     |
| Stores        | 69242205 | 4.58%     |
| NOPs          | 179676    | 0.01%      |
| Direct Calls   | 3278122  | 0.22%      |
| Indirect Calls  | 511   | 0.00%      |
| Returns          | 3278634  | 0.22%      |
| Unconditional Branches    | 5487101  | 0.36%      |
| Conditional Branches      | 112770156 | 7.47%      |
| Logical operations      | 35272619 | 2.34%      |
| Rotate and Shift | 264878   | 0.02%      |
| Flag Operations       | 4318691    | 0.29%      |
| Vector Instructions       | 0         | 0.00%      |
| Conditional Moves         | 0         | 0.00%      |
| MMX and SSE instructions      | 0         | 0.00%      |
| System Calls      | 0         | 0.00%      |
| Floating Point Instructions          | 412503500    | 27.31%      |
| The rest         | 422646109 | 27.98%     |

#### Part B
**CPI:** 24.31672

#### Part C
**Data Chunk Accesses:** 154004 \
**Instruction Chunk Accesses:** 1173 

#### Part D
**Distribution of Instruction Length**
| Bytes | Count       |
|-------|------------|
| 0     | 0          |
| 1     | 81214950   |
| 2     | 430545481  |
| 3     | 432113066  |
| 4     | 26290007   |
| 5     | 7126668    |
| 6     | 17912431   |
| 7     | 4753425    |
| 8     | 43956      |
| 9     | 0          |
| 10    | 13         |
| 11    | 0          |
| 12    | 0          |
| 13    | 0          |
| 14    | 0          |
| 15    | 0          |

**Distribution of the number of operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 179676      |
| 1        | 7759       |
| 2        | 301660182  |
| 3        | 502104074  |
| 4        | 179681579   |
| 5        | 16290903   |
| 6        | 75824   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of register read operands in an instruction**
| Reads | Count       |
|----------|------------|
| 0        | 5015943      |
| 1        | 112978870       |
| 2        | 585929061  |
| 3        | 151630334  |
| 4        | 131357184   |
| 5        | 13012781   |
| 6        | 75824   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of register write operands in an instruction**
| Writes | Count       |
|----------|------------|
| 0        | 45850688      |
| 1        | 833895779       |
| 2        | 120176297  |
| 3        | 77233  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 50870569      |
| 1        | 480373250       |
| 2        | 10921050  |
| 3        | 0  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |


**Distribution of the number of memory read operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 55474540      |
| 1        | 435819760       |
| 2        | 0  |
| 3        | 0  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Distribution of the number of memory write operands in an instruction**
| Operands | Count       |
|----------|------------|
| 0        | 424898710      |
| 1        | 66395590       |
| 2        | 0  |
| 3        | 0  |
| 4        | 0   |
| 5        | 0   |
| 6        | 0   |
| 7        | 0          |
| 8        | 0          |
| 9        | 0          |

**Maximum number of bytes touched by an instruction:** 8 \ 
**Average number of bytes touched by an instruction:** 3.99254 \
**Maximum value of immediate:** 2147483647 \
**Minimum value of immediate:** -2147483648 \
**Maximum value of displacement used in memory addressing:** 135127364 \
**Minimum value of displacement used in memory addressing:** -121088