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
| Instruction Count      | 999999999 | -          |
| Loads         | 356838743 | 22.83%     |
| Stores        | 205970465 | 13.18%     |
| NOPs          | 959563    | 0.06%      |
| Direct Calls   | 12742485  | 0.82%      |
| Indirect Calls  | 2839560   | 0.18%      |
| Returns          | 15582046  | 1.00%      |
| Unconditional Branches    | 30549008  | 1.95%      |
| Conditional Branches      | 129984041 | 8.32%      |
| Logical operations      | 100106278 | 6.41%      |
| Rotate and Shift | 4275188   | 0.27%      |
| Flag Operations       | 862091    | 0.06%      |
| Vector Instructions       | 0         | 0.00%      |
| Conditional Moves         | 0         | 0.00%      |
| MMX and SSE instructions      | 0         | 0.00%      |
| System Calls      | 0         | 0.00%      |
| Floating Point Instructions          | 933010    | 0.06%      |
| The rest         | 701166729 | 44.87%     |

#### Part B
**CPI:** 25.85

#### Part C
**Data Chunk Accesses:** 31106 \
**Instruction Chunk Accesses:** 2810

#### Part D
**Distribution of Instruction Length**
| Bytes | Count       |
|-------|------------|
| 0     | 0          |
| 1     | 117382739  |
| 2     | 256612066  |
| 3     | 274772730  |
| 4     | 52905377   |
| 5     | 78567185   |
| 6     | 185781332  |
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
| 2        | 520827029  |
| 3        | 355062688  |
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
| 1     | 260353244  |
| 2     | 537426193  |
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
| 1      | 685177101  |
| 2      | 175739722  |
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
| 0        | 453635187  |
| 1        | 530799021  |
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
| 1        | 354817363  |
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
| 0        | 341028202  |
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
**Average number of bytes touched by an instruction:** 3.53\
**Maximum value of immediate:** 2147483647\
**Minimum value of immediate:** -2147483648\
**Maximum value of displacement used in memory addressing:** 135918104\
**Minimum value of displacement used in memory addressing:** -1408

### 401.bzip2 input.source
#### Part A

#### Part B
**CPI:** 

#### Part C
**Data Chunk Accesses:**
**Instruction Chunk Accesses:**

#### Part D
**Distribution of Instruction Length**

**Distribution of the number of operands in an instruction**

**Distribution of the number of register read operands in an instruction**

**Distribution of the number of register write operands in an instruction**

**Distribution of the number of memory operands in an instruction**

**Distribution of the number of memory read operands in an instruction**

**Distribution of the number of memory write operands in an instruction**

**Maximum number of bytes touched by an instruction:** 

**Average number of bytes touched by an instruction:**

**Maximum value of immediate:**

**Minimum value of immediate:**

**Maximum value of displacement used in memory addressing:**

**Minimum value of displacement used in memory addressing:**

### 403.gcc cp-decl.i
#### Part A

#### Part B
**CPI:** 

#### Part C
**Data Chunk Accesses:**
**Instruction Chunk Accesses:**

#### Part D
**Distribution of Instruction Length**

**Distribution of the number of operands in an instruction**

**Distribution of the number of register read operands in an instruction**

**Distribution of the number of register write operands in an instruction**

**Distribution of the number of memory operands in an instruction**

**Distribution of the number of memory read operands in an instruction**

**Distribution of the number of memory write operands in an instruction**

**Maximum number of bytes touched by an instruction:** 

**Average number of bytes touched by an instruction:**

**Maximum value of immediate:**

**Minimum value of immediate:**

**Maximum value of displacement used in memory addressing:**

**Minimum value of displacement used in memory addressing:**

### 429.mcf
#### Part A

#### Part B
**CPI:** 

#### Part C
**Data Chunk Accesses:**
**Instruction Chunk Accesses:**

#### Part D
**Distribution of Instruction Length**

**Distribution of the number of operands in an instruction**

**Distribution of the number of register read operands in an instruction**

**Distribution of the number of register write operands in an instruction**

**Distribution of the number of memory operands in an instruction**

**Distribution of the number of memory read operands in an instruction**

**Distribution of the number of memory write operands in an instruction**

**Maximum number of bytes touched by an instruction:** 

**Average number of bytes touched by an instruction:**

**Maximum value of immediate:**

**Minimum value of immediate:**

**Maximum value of displacement used in memory addressing:**

**Minimum value of displacement used in memory addressing:**

### 450.soplex ref.mps

#### Part A

#### Part B
**CPI:** 

#### Part C
**Data Chunk Accesses:**
**Instruction Chunk Accesses:**

#### Part D
**Distribution of Instruction Length**

**Distribution of the number of operands in an instruction**

**Distribution of the number of register read operands in an instruction**

**Distribution of the number of register write operands in an instruction**

**Distribution of the number of memory operands in an instruction**

**Distribution of the number of memory read operands in an instruction**

**Distribution of the number of memory write operands in an instruction**

**Maximum number of bytes touched by an instruction:** 

**Average number of bytes touched by an instruction:**

**Maximum value of immediate:**

**Minimum value of immediate:**

**Maximum value of displacement used in memory addressing:**

**Minimum value of displacement used in memory addressing:**

### 456.hmmer nph3.hmm
#### Part A

#### Part B
**CPI:** 

#### Part C
**Data Chunk Accesses:**
**Instruction Chunk Accesses:**

#### Part D
**Distribution of Instruction Length**

**Distribution of the number of operands in an instruction**

**Distribution of the number of register read operands in an instruction**

**Distribution of the number of register write operands in an instruction**

**Distribution of the number of memory operands in an instruction**

**Distribution of the number of memory read operands in an instruction**

**Distribution of the number of memory write operands in an instruction**

**Maximum number of bytes touched by an instruction:** 

**Average number of bytes touched by an instruction:**

**Maximum value of immediate:**

**Minimum value of immediate:**

**Maximum value of displacement used in memory addressing:**

**Minimum value of displacement used in memory addressing:**

### 471.omnetpp
#### Part A

#### Part B
**CPI:** 

#### Part C
**Data Chunk Accesses:**
**Instruction Chunk Accesses:**

#### Part D
**Distribution of Instruction Length**

**Distribution of the number of operands in an instruction**

**Distribution of the number of register read operands in an instruction**

**Distribution of the number of register write operands in an instruction**

**Distribution of the number of memory operands in an instruction**

**Distribution of the number of memory read operands in an instruction**

**Distribution of the number of memory write operands in an instruction**

**Maximum number of bytes touched by an instruction:** 

**Average number of bytes touched by an instruction:**

**Maximum value of immediate:**

**Minimum value of immediate:**

**Maximum value of displacement used in memory addressing:**

**Minimum value of displacement used in memory addressing:**

### 483.xalancbmk
#### Part A

#### Part B
**CPI:** 

#### Part C
**Data Chunk Accesses:**
**Instruction Chunk Accesses:**

#### Part D
**Distribution of Instruction Length**

**Distribution of the number of operands in an instruction**

**Distribution of the number of register read operands in an instruction**

**Distribution of the number of register write operands in an instruction**

**Distribution of the number of memory operands in an instruction**

**Distribution of the number of memory read operands in an instruction**

**Distribution of the number of memory write operands in an instruction**

**Maximum number of bytes touched by an instruction:** 

**Average number of bytes touched by an instruction:**

**Maximum value of immediate:**

**Minimum value of immediate:**

**Maximum value of displacement used in memory addressing:**

**Minimum value of displacement used in memory addressing:**

## Optional Benchmarks

### 436.cactusADM
#### Part A

#### Part B
**CPI:** 

#### Part C
**Data Chunk Accesses:**
**Instruction Chunk Accesses:**

#### Part D
**Distribution of Instruction Length**

**Distribution of the number of operands in an instruction**

**Distribution of the number of register read operands in an instruction**

**Distribution of the number of register write operands in an instruction**

**Distribution of the number of memory operands in an instruction**

**Distribution of the number of memory read operands in an instruction**

**Distribution of the number of memory write operands in an instruction**

**Maximum number of bytes touched by an instruction:** 

**Average number of bytes touched by an instruction:**

**Maximum value of immediate:**

**Minimum value of immediate:**

**Maximum value of displacement used in memory addressing:**

**Minimum value of displacement used in memory addressing:**

### 437.leslie3d
#### Part A

#### Part B
**CPI:** 

#### Part C
**Data Chunk Accesses:**
**Instruction Chunk Accesses:**

#### Part D
**Distribution of Instruction Length**

**Distribution of the number of operands in an instruction**

**Distribution of the number of register read operands in an instruction**

**Distribution of the number of register write operands in an instruction**

**Distribution of the number of memory operands in an instruction**

**Distribution of the number of memory read operands in an instruction**

**Distribution of the number of memory write operands in an instruction**

**Maximum number of bytes touched by an instruction:** 

**Average number of bytes touched by an instruction:**

**Maximum value of immediate:**

**Minimum value of immediate:**

**Maximum value of displacement used in memory addressing:**

**Minimum value of displacement used in memory addressing:**

### 462.libquantum
#### Part A

#### Part B
**CPI:** 

#### Part C
**Data Chunk Accesses:**
**Instruction Chunk Accesses:**

#### Part D
**Distribution of Instruction Length**

**Distribution of the number of operands in an instruction**

**Distribution of the number of register read operands in an instruction**

**Distribution of the number of register write operands in an instruction**

**Distribution of the number of memory operands in an instruction**

**Distribution of the number of memory read operands in an instruction**

**Distribution of the number of memory write operands in an instruction**

**Maximum number of bytes touched by an instruction:** 

**Average number of bytes touched by an instruction:**

**Maximum value of immediate:**

**Minimum value of immediate:**

**Maximum value of displacement used in memory addressing:**

**Minimum value of displacement used in memory addressing:**

### 470.lbm
#### Part A

#### Part B
**CPI:** 

#### Part C
**Data Chunk Accesses:**
**Instruction Chunk Accesses:**

#### Part D
**Distribution of Instruction Length**

**Distribution of the number of operands in an instruction**

**Distribution of the number of register read operands in an instruction**

**Distribution of the number of register write operands in an instruction**

**Distribution of the number of memory operands in an instruction**

**Distribution of the number of memory read operands in an instruction**

**Distribution of the number of memory write operands in an instruction**

**Maximum number of bytes touched by an instruction:** 

**Average number of bytes touched by an instruction:**

**Maximum value of immediate:**

**Minimum value of immediate:**

**Maximum value of displacement used in memory addressing:**

**Minimum value of displacement used in memory addressing:**

### 482.sphinx3

#### Part A

#### Part B
**CPI:** 

#### Part C
**Data Chunk Accesses:**
**Instruction Chunk Accesses:**

#### Part D
**Distribution of Instruction Length**

**Distribution of the number of operands in an instruction**

**Distribution of the number of register read operands in an instruction**

**Distribution of the number of register write operands in an instruction**

**Distribution of the number of memory operands in an instruction**

**Distribution of the number of memory read operands in an instruction**

**Distribution of the number of memory write operands in an instruction**

**Maximum number of bytes touched by an instruction:** 

**Average number of bytes touched by an instruction:**

**Maximum value of immediate:**

**Minimum value of immediate:**

**Maximum value of displacement used in memory addressing:**

**Minimum value of displacement used in memory addressing:**