# CS422: Computer Architecture
# Homework 2

## Implementation Details

The final results are reported on a server with 8 threads, and 32 GB RAM. The processor used is - AMD EPYC-Milan Processor

All the fractions have been listed as percentages.
<div style="page-break-after: always;"></div>

## Benchmark Results

### 400.perlbench diffmail.pl
#### Part A
| Predictor | Total Predictions | Total Misprediction | Forward Misprediction | Backward Misprediction |
|--------------|-----------|------------|------------|------------|
| FNBT                | 130101783 | 41.4324% | 37.7532% | 56.2276% |
| Bimodal             | 130101783 | 10.5119% | 11.3614% | 7.0959% |
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
| Bimodal             | 129923039 | 11.2117% | 12.3359% | 10.1476% |
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
| FNBT                | 145808740 | 36.5862% | 31.8032% | 54.1118% |
| Bimodal             | 145808740 | 15.4525% | 18.2661% | 5.14271% |
| SAg                 | 145808740 | 5.09578% | 5.36626% | 4.10469% |
| GAg                 | 145808740 | 15.4023% | 16.1541% | 12.6474% |
| gshare              | 145808740 | 9.73661% | 9.1345%  | 11.9429% |
| Hybrid-1            | 145808740 | 4.19471% | 4.51028% | 3.03827% |
| Hybrid-2 Majority   | 145808740 | 5.39252% | 5.35559% | 5.52786% |
| Hybrid-2 Tournament | 145808740 | 4.70586% | 4.79012% | 4.39711% |

#### Part B
| BTB Type | BTB Predictions | BTB Miss Rate | BTB Missprediction |
|--------------|-----------|------------|------------|
| BTB indexed with PC                            | 34741834 | 0.000109% | 70.62555% |
| BTB indexed with hash of PC and global history | 34741834 | 0.047634% | 31.75576% |

### 429.mcf
#### Part A
| Predictor | Total Predictions | Total Misprediction | Forward Misprediction | Backward Misprediction |
|--------------|-----------|------------|------------|------------|
| FNBT                | 172842909 | 31.9495% | 35.7088% | 28.1899% |
| Bimodal             | 172842909 | 22.3749% | 20.4881% | 24.2619% |
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
| BTB indexed with hash of PC and global history | 12556349 | 0.0008632% | 0.40991% |

### 450.soplex ref.mps
#### Part A
| Predictor | Total Predictions | Total Misprediction | Forward Misprediction | Backward Misprediction |
|--------------|-----------|------------|------------|------------|
| FNBT                | 103219361 | 17.0122% | 19.9393% | 15.5894% |
| Bimodal             | 103219361 | 4.97406% | 1.03329% | 6.88967% |
| SAg                 | 103219361 | 4.02024% | 0.657045% | 5.65509% |
| GAg                 | 103219361 | 3.81006% | 0.849235% | 5.24932% |
| gshare              | 103219361 | 3.98045% | 1.29791% | 5.28444% |
| Hybrid-1            | 103219361 | 3.58499% | 0.650398% | 5.0115% |
| Hybrid-2 Majority   | 103219361 | 3.79203% | 0.819812% | 5.23683% |
| Hybrid-2 Tournament | 103219361 | 3.66753% | 0.677946% | 5.12078% |
#### Part B
| BTB Type | BTB Predictions | BTB Miss Rate | BTB Missprediction |
|--------------|-----------|------------|------------|
| BTB indexed with PC                            | 6315936 | 0.0014883% | 0.005351% |
| BTB indexed with hash of PC and global history | 6315936 | 0.0053990% | 0.010465% |

### 456.hmmer nph3.hmm
#### Part A
| Predictor | Total Predictions | Total Misprediction | Forward Misprediction | Backward Misprediction |
|--------------|-----------|------------|------------|------------|
| FNBT                | 144361424 | 63.9146% | 76.6315% | 0.669822% |
| Bimodal             | 144361424 | 8.73992% | 10.4269% | 0.350169% |
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
| Bimodal             | 117335288 | 13.5703% | 13.6177% | 13.3239% |
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
| Bimodal             | 181215584 | 4.49732% | 5.25642% | 2.32969% |
| SAg                 | 181215584 | 1.85848% | 2.10205% | 1.16294% |
| GAg                 | 181215584 | 4.52388% | 5.04071% | 3.04806% |
| gshare              | 181215584 | 3.929%   | 4.28487% | 2.91279% |
| Hybrid-1            | 181215584 | 1.40294% | 1.53529% | 1.025% |
| Hybrid-2 Majority   | 181215584 | 2.12208% | 2.29935% | 1.61585% |
| Hybrid-2 Tournament | 181215584 | 1.70337% | 1.90671% | 1.12271% |
#### Part B
| BTB Type | BTB Predictions | BTB Miss Rate | BTB Missprediction |
|--------------|-----------|------------|------------|
| BTB indexed with PC                            | 32447858 | 2.656661% | 27.51067% |
| BTB indexed with hash of PC and global history | 32447858 | 15.440501% | 27.94729% |

