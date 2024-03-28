# Copy Models for Data Compression (Group 2)
## Build Project

To build our project, a build.sh script was created in the root group_2 which will compile the necessary programs for the project, namely cpm and mutate

```bash
./build.sh
```

## CPM

To run __CPM__, you must run the following program with the following arguments:

- `-f` to indicate the name of the file to be calculated
- `-k` to specify the size of the k-mer window
- `-t` to set the threshold and minTries to check the model threshold in the format \<threshold>/\<minTries>
- `-n` to set the maximum number of simultaneous copymodel references
- `-a` to set the value of smoothing alpha
- `-s` to specify the size of the window to be used in the Fallback Model
- `-m` to indicate the minimum number of consecutive letters required to consider a match between k-mers

### Example of usage

```bash
./bin/cpm -f example/chry.txt -k 8 -t 8/0.8 -n 1 -s 200 -m 4 -a 1
```

## Mutate

To run __Mutate__, you must run the following program with the following arguments:

- `-i` to indicate the name of the input file with the sequence to be mutated
- `-o` to indicate the name of the output file where it will be saved the mutated sequence
- `-p` to indicate the probability of a symbol of the sequence to be changed


### Example of usage

```bash
./bin/mutate -i example/chry.txt -o chry_mutated.txt -p 0.5
```
