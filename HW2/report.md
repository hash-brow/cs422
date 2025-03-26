# CS422: Computer Architecture
# Homework 2

## Implementation Details

Part A code is contained in `predictor.cpp` and `predictor.h`. All predictors are derived from the base class `Predictor` and the actual prediction logic is implemented in an overloaded `operator+`. SAg, GAg and gshare results are passed to the hybrid predictors as a `std::optional` in the `data_t` struct. To reconfigure the sizes of the tables and/or the counters, relevant constants in `predictor.h` should be modified.

The final results are reported on a server with 8 threads, and 32 GB RAM. The processor used is - AMD EPYC-Milan Processor

All the fractions have been listed as percentages.
<div style="page-break-after: always;"></div>

## Results Explanation - Part A

We will have a brief look at the various predictors, starting from worst to best.

The overall worst is of course FNBT, as it does not really learn anything - it can only be successful in certain specific scenarios. One would expect the majority of branches in a program to be from loops, and if the loops have a large iterations/exits ratio and the conditional branch is backward FNBT can work decently well. For eg., the backward misprediction rate for FNBT + 456.hmmer is <1% despite a significant number of backward branches. Obviously generating such code is a huge constraint and is generally not feasible from the compiler's standpoint.

The next worst predictors are GAg, gshare and the bimodal predictor. Which one works better depends on the specific benchmark. It is clear why bimodal is not such a good predictor, as it can only predict branches which have a high degree of certainty towards a certain outcome. Even then, for example in a loop which exits after 5 iterations, the bimodal predictor will have a misprediction every 5'th time, whereas any history-based predictor will not. 

Obviously using global history will help certain kind of branches, whereas there will be branches which will highly correlate with their local history instead of the global one. The usual trend of misprediction rates is `GAg > gshare >> SAg`. From this, we hypothesize that most branches correlate more with their local histories than global ones. GAg and gshare have similar misprediction rates - however as gshare avoids aliasing, i.e because of the XOR it can differentiate b/w branches with the same history, it is slightly better than GAg on average.

SAg maintains the local history of branches in a table which allows for an accurate record of the local histories with very low aliasing. This combined with our hypothesis explains why SAg is clearly better than GAg and gshare.

The hybrid predictors are the most accurate, as one would expect. The majority predictor is the worst amongst the three, and is sometimes even worse than SAg - this is expected as gshare and GAg will produce somewhat similar results. What was surprising to us is that the majority predictor still works fairly well, which would mean that gshare and GAg mispredictions are often on different kind of branches. 

As an example, we can look at 403.gcc. The misprediction rates are `SAg = 5%, GAg = 15%, gshare = 9%, majority = 5.4%`. This is an extreme example, but nevertheless we can see that gshare performs much better than GAg, which shows that aliasing is a big issue for certain applications, and that also explains why a simple majority works pretty well despite GAg and gshare having the same fundamental idea.

The hybrid b/w SAg and GAg and the tournament predictor have similar results. For the former, we update the meta-table only when SAg and GAg have different outcomes. This seems the obvious way to us.

However, upon doing this with the tournament predictor, i.e updating all 3 meta-tables simulatenously but updating a table only when the two predictors it is comparing have unequal predictions, we achieved results better than the given reference output. To match with the reference output, we have changed our implementation to update all 3 tables irrespective of whether the predictors a meta-table is comparing have equal results.

This has given slightly worse results. As the logic b/w the SAG + GAg hybrid and the tournament predictor is now fundamentally different, there is not much point trying to compare their mostly similar results. They both work better than a simple majority as they are able to learn the most suitable predictor for every branch.

## Results Explanation - Part B

## Benchmark Results

### 400.perlbench diffmail.pl
#### Part A
| Predictor | Total Predictions | Total Misprediction | Forward Misprediction | Backward Misprediction |
|--------------|-----------|------------|------------|------------|
| FNBT                | 130101783 | 41.4324% | 37.7532% | 56.2276% |
| Bimodal             | 130101783 | 9.43105% | 9.7841%  | 8.01129% |
| SAg                 | 130101783 | 3.62852% | 3.70322% | 3.32812% |
| GAg                 | 130101783 | 11.6579% | 12.324%  | 8.97914% |
| gshare              | 130101783 | 10.0557% | 10.1364% | 9.73105% |
| Hybrid-1            | 130101783 | 3.09021% | 3.17035% | 2.7679% |
| Hybrid-2 Majority   | 130101783 | 4.9575%  | 5.01487% | 4.72681% |
| Hybrid-2 Tournament | 130101783 | 3.9031%  | 4.12987% | 2.99119% |
#### Part B
| BTB Type | BTB Predictions | BTB Miss Rate | BTB Missprediction |
|--------------|-----------|------------|------------|
| BTB indexed with PC                            | 28134124 | 0.01209% | 34.67776% |
| BTB indexed with hash of PC and global history | 28134124 | 1.78012% | 10.22728% |

### 401.bzip2 input.source
#### Part A
| Predictor | Total Predictions | Total Misprediction | Forward Misprediction | Backward Misprediction |
|--------------|-----------|------------|------------|------------|
| FNBT                | 129923039 | 46.8918% | 30.9891% | 61.9444% |
| Bimodal             | 129923039 | 9.99838% | 10.7903% | 9.24879% |
| SAg                 | 129923039 | 10.1409% | 11.4241% | 8.92617% |
| GAg                 | 129923039 | 12.5105% | 14.7517% | 10.389% |
| gshare              | 129923039 | 11.3094% | 12.1151% | 10.5467% |
| Hybrid-1            | 129923039 | 9.5824%  | 10.8428% | 8.38938% |
| Hybrid-2 Majority   | 129923039 | 9.3016%  | 10.201%  | 8.45027% |
| Hybrid-2 Tournament | 129923039 | 9.5726%  | 10.6397% | 8.56253% |

#### Part B
| BTB Type | BTB Predictions | BTB Miss Rate | BTB Missprediction |
|--------------|-----------|------------|------------|
| BTB indexed with PC                            | 791909 | 0.00745% | 48.29393% |
| BTB indexed with hash of PC and global history | 791909 | 0.02702% | 47.52377% |

### 403.gcc cp-decl.i
#### Part A
| Predictor | Total Predictions | Total Misprediction | Forward Misprediction | Backward Misprediction |
|--------------|-----------|------------|------------|------------|
| FNBT                | 145814336 | 36.5858% | 31.8015% | 54.1155% |
| Bimodal             | 145814336 | 12.9411% | 14.7189% | 6.4272% |
| SAg                 | 145814336 | 5.09652% | 5.36705% | 4.10526% |
| GAg                 | 145814336 | 15.4023% | 16.1536% | 12.6497% |
| gshare              | 145814336 | 9.73444% | 9.13051%  | 11.9473% |
| Hybrid-1            | 145814336 | 4.19446% | 4.50981% | 3.03896% |
| Hybrid-2 Majority   | 145814336 | 5.39209% | 5.35454% | 5.52966% |
| Hybrid-2 Tournament | 145814336 | 4.71379% | 4.79988% | 4.39832% |

#### Part B
| BTB Type | BTB Predictions | BTB Miss Rate | BTB Missprediction |
|--------------|-----------|------------|------------|
| BTB indexed with PC                            | 34737233 | 0.000106% | 70.62674% |
| BTB indexed with hash of PC and global history | 34737233 | 0.045139% | 31.76835% |

### 429.mcf
#### Part A
| Predictor | Total Predictions | Total Misprediction | Forward Misprediction | Backward Misprediction |
|--------------|-----------|------------|------------|------------|
| FNBT                | 172842909 | 31.9495% | 35.7088% | 28.1899% |
| Bimodal             | 172842909 | 18.02%   | 16.2883% | 19.7519% |
| SAg                 | 172842909 | 13.0502% | 15.0982% | 11.0021% |
| GAg                 | 172842909 | 9.28587% | 9.22752% | 9.34423% |
| gshare              | 172842909 | 10.2195% | 10.3481% | 10.0909% |
| Hybrid-1            | 172842909 | 8.7753%  | 8.94987% | 8.60071% |
| Hybrid-2 Majority   | 172842909 | 8.68154% | 8.7609%  | 8.60217% |
| Hybrid-2 Tournament | 172842909 | 8.88845% | 9.08998% | 8.68689% |
#### Part B
| BTB Type | BTB Predictions | BTB Miss Rate | BTB Missprediction |
|--------------|-----------|------------|------------|
| BTB indexed with PC                            | 12556349 | 0.000072% | 0.61349% |
| BTB indexed with hash of PC and global history | 12556349 | 0.0008362% | 0.40991% |

### 450.soplex ref.mps
#### Part A
| Predictor | Total Predictions | Total Misprediction | Forward Misprediction | Backward Misprediction |
|--------------|-----------|------------|------------|------------|
| FNBT                | 103219361 | 17.0122% | 19.9393%  | 15.5894% |
| Bimodal             | 103219361 | 4.8278%  | 0.910829% | 6.73185% |
| SAg                 | 103219361 | 4.02024% | 0.657045% | 5.65509% |
| GAg                 | 103219361 | 3.81006% | 0.849235% | 5.24932% |
| gshare              | 103219361 | 3.98045% | 1.29791%  | 5.28444% |
| Hybrid-1            | 103219361 | 3.58499% | 0.650398% | 5.0115% |
| Hybrid-2 Majority   | 103219361 | 3.79203% | 0.819812% | 5.23683% |
| Hybrid-2 Tournament | 103219361 | 3.66753% | 0.677946% | 5.12078% |
#### Part B
| BTB Type | BTB Predictions | BTB Miss Rate | BTB Missprediction |
|--------------|-----------|------------|------------|
| BTB indexed with PC                            | 6315969 | 0.0014883% | 0.005351% |
| BTB indexed with hash of PC and global history | 6315969 | 0.0053990% | 0.010465% |

### 456.hmmer nph3.hmm
#### Part A
| Predictor | Total Predictions | Total Misprediction | Forward Misprediction | Backward Misprediction |
|--------------|-----------|------------|------------|------------|
| FNBT                | 144361424 | 63.9146% | 76.6315% | 0.669822% |
| Bimodal             | 144361424 | 8.55505% | 10.2041% | 0.353847% |
| SAg                 | 144361424 | 9.12542% | 10.8637% | 0.480347% |
| GAg                 | 144361424 | 11.7689% | 13.6194% | 2.56574% |
| gshare              | 144361424 | 10.2741% | 11.7958% | 2.70623% |
| Hybrid-1            | 144361424 | 8.64265% | 10.2256% | 0.770076% |
| Hybrid-2 Majority   | 144361424 | 8.68884% | 10.3102% | 0.625552% |
| Hybrid-2 Tournament | 144361424 | 8.68196% | 10.3029% | 0.620389% |
#### Part B
| BTB Type | BTB Predictions | BTB Miss Rate | BTB Missprediction |
|--------------|-----------|------------|------------|
| BTB indexed with PC                            | 201570 | 0.056556% | 6.385375% |
| BTB indexed with hash of PC and global history | 201570 | 0.4216897% | 2.64077% |

### 471.omnetpp
#### Part A
| Predictor | Total Predictions | Total Misprediction | Forward Misprediction | Backward Misprediction |
|--------------|-----------|------------|------------|------------|
| FNBT                | 117335288 | 34.1219% | 33.7247% | 36.1847% |
| Bimodal             | 117335288 | 10.3997% | 9.72771% | 13.8898% |
| SAg                 | 117335288 | 5.003%   | 4.36208% | 8.33151% |
| GAg                 | 117335288 | 12.2328% | 11.8266% | 14.3424% |
| gshare              | 117335288 | 10.8435% | 10.3791% | 13.2551% |
| Hybrid-1            | 117335288 | 4.11894% | 3.584%   | 6.89712% |
| Hybrid-2 Majority   | 117335288 | 5.48414% | 4.76363% | 9.22605% |
| Hybrid-2 Tournament | 117335288 | 5.02242% | 4.3817%  | 8.34993% |
#### Part B
| BTB Type | BTB Predictions | BTB Miss Rate | BTB Missprediction |
|--------------|-----------|------------|------------|
| BTB indexed with PC                            | 30294701 | 0.018828% | 29.360996% |
| BTB indexed with hash of PC and global history | 30294701 | 0.878609% | 11.637649% |

### 483.xalancbmk
#### Part A
| Predictor | Total Predictions | Total Misprediction | Forward Misprediction | Backward Misprediction |
|--------------|-----------|------------|------------|------------|
| FNBT                | 181215584 | 7.80045% | 8.80922% | 4.91988% |
| Bimodal             | 181215584 | 3.52299% | 4.03331% | 2.06576% |
| SAg                 | 181215584 | 1.85848% | 2.10205% | 1.16294% |
| GAg                 | 181215584 | 4.52388% | 5.04071% | 3.04806% |
| gshare              | 181215584 | 3.929%   | 4.28487% | 2.91279% |
| Hybrid-1            | 181215584 | 1.40294% | 1.53529% | 1.025% |
| Hybrid-2 Majority   | 181215584 | 2.12208% | 2.29935% | 1.61585% |
| Hybrid-2 Tournament | 181215584 | 1.70337% | 1.90671% | 1.12271% |
#### Part B
| BTB Type | BTB Predictions | BTB Miss Rate | BTB Missprediction |
|--------------|-----------|------------|------------|
| BTB indexed with PC                            | 32447858 | 2.356661% | 27.51067% |
| BTB indexed with hash of PC and global history | 32447858 | 15.440501% | 27.94729% |

