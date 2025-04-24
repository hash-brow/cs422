# CS422: Computer Architecture
# Homework 3 

## Implementation Details

All the fractions have been listed as percentages.
<div style="page-break-after: always;"></div>

## Results Explanation - Part A

## Results Explanation - Part B

#### Part A Results

| Test Case    | Total No. of Instructions | Loads (%) | Stores (%) | Conditional Branches (%) |
|--------------|-----------|------------|------------|------------|
| asm-sim      | 158   | 39.24050633% | 34.81012658% | 0% |
| c-sim| 1928  | 17.42738589% | 8.19502075%  | 19.29460581% |
| endian       | 1965  | 19.13486005% | 11.043257%   | 18.01526717% |
| factorial    | 2530  | 11.10671937% | 8.61660079%  | 18.41897233% |
| fib  | 44903 | 13.59151949% | 17.77609514% | 8.34465403%  |
| hello| 1744  | 19.094037%   | 8.60091743%  | 19.61009174% |
| host | 14894 | 20.10205452% | 9.92345911%  | 19.22250571% |
| ifactorial   | 2420  | 16.48760331% | 7.85123967%  | 19.25619835% |
| ifib | 7581  | 16.98984303% | 10.01187178% | 17.50428703% |
| log2 | 2096  | 17.89122137% | 8.34923664%  | 19.60877863% |
| msort| 17219 | 10.58714211% | 5.44166328%  | 18.12532667% |
| rfib | 25668 | 14.39925199% | 17.02898551% | 9.51379149% |
| subreg       | 4361  | 21.64641137% | 16.28066957% | 15.59275395% |
| towers       | 82763 | 20.05727197% | 8.48809250%  | 20.54299327% |
| vadd | 7259  | 6.14409698%  | 16.35211462% | 10.57996969% |

#### Part B Results

Design-1: Design with complete interlock logic, no bypass paths and two branch delay slots
Design-2: Design-1 with only one branch delay slot. (Branch interlock cycle removed)
Design-3: Design-2 with EX-EX bypass path enabled
Design-4: Design-3 with MEM-MEM bypass path enabled
Final Design: THis is the final design with single branch delay slot and all bypass paths enabled (EX-EX, MEM-MEM, MEM-EX)

CPI of all these designs is given in the table below:

| Test Case  | Design-1 | Design-2 | Design-3 | Design-4 | Final Design |
|------------|----------|----------|----------|----------|--------------|
| asm-sim    | 1.57 | 1.47 | 1.30 | 1.22 | 1.22 |
| c-sim      | 1.85 | 1.65 | 1.14 | 1.01 | 1.01 |
| endian     | 1.78 | 1.58 | 1.14 | 1.01 | 1.01 |
| factorial  | 1.82 | 1.63 | 1.14 | 1.01 | 1.01 |
| fib        | 1.52 | 1.41 | 1.18 | 1.00 | 1.00 |
| hello      | 1.81 | 1.62 | 1.15 | 1.01 | 1.01 |
| host       | 1.60 | 1.45 | 1.15 | 1.01 | 1.01 |
| ifactorial | 1.83 | 1.65 | 1.14 | 1.01 | 1.01 |
| ifib       | 1.71 | 1.54 | 1.14 | 1.00 | 1.00 |
| log2       | 1.78 | 1.59 | 1.14 | 1.01 | 1.01 |
| msort      | 1.92 | 1.72 | 1.05 | 1.00 | 1.00 |
| rfib       | 1.54 | 1.43 | 1.18 | 1.00 | 1.00 |
| subreg     | 1.58 | 1.43 | 1.14 | 1.01 | 1.01 |
| towers     | 1.55 | 1.41 | 1.15 | 1.00 | 1.00 |
| vadd       | 1.98 | 1.88 | 1.15 | 1.00 | 1.00 |

The Load-Interlock stall cycles and syscalls with respect to total no. of instructions in the final design are given below:

| Test Case  | No. of Instructions | Syscalls(%) | Load-Interlock Stalls(%) | Effective CPI |
|------------|---------------------|-------------|-------------|------------|
| asm-sim    | 50    | 0.06%      | 0.00%      | 1.22 |
| c-sim      | 1826  | 0.0027383% | 0.0777656% | 1.10 |
| endian     | 1865  | 0.002681%  | 0.0761394% | 1.09 |
| factorial  | 2408  | 0.0020764% | 0.0660299% | 1.08 |
| fib        | 44780 | 0.0001117% | 0.0098928% | 1.01 |
| hello      | 1638  | 0.0030525% | 0.0854701% | 1.11 |
| host       | 14520 | 0.0016529% | 0.0597107% | 1.07 |
| ifactorial | 2293  | 0.0021805% | 0.0693415% | 1.08 |
| ifib       | 7453  | 0.0006709% | 0.0594391% | 1.07 |
| log2       | 1996  | 0.002505%  | 0.0786573% | 1.10 |
| msort      | 17103 | 0.0002923% | 0.0382974% | 1.04 |
| rfib       | 25541 | 0.0001958% | 0.0173447% | 1.02 |
| subreg     | 4224  | 0.0014204% | 0.055161%  | 1.06 |
| towers     | 80652 | 0.0008307% | 0.0583866% | 1.06 |
| vadd       | 7130  | 0.0007013% | 0.0248247% | 1.03 |


