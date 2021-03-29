<!--- 
Name: Anirudh S
Roll no: CS17B003
-->

# Assignment 2: TCP congestion control

## Requirements
Running the plotting script requires `python3` and `matplotlib` library

## Instructions to run the code
Please follow the instructions given below to compile and execute the code. All commands given here are `bash` commands.

### Running the script
The script simulates the AIMD algorithm and plots the corresponding graph. There is provision to run for all parameter combinations specified in the assignment or any user determined values.

The script accepts the following flags:
1. `-a`: Run the simulator for all parameter combinations; if this flag is passed all other flags are ignored. The simulator outputs are stored in the `outputs` directory and the corresponding graphs in `graphs` directory.
2. `-i`: Initial CW size (in MSS); type: `float`, default value is 1; range: [1, 4]
3. `-m`: Exponential growth multiplier; type: `float`, default value is 1; range: [0.5, 2]
4. `-n`: Linear growth multiplier; type: `float`, default value is 1; range: [0.5, 2]
5. `-f`: Timeout multiplier; type: `float`; range: [0.1, 0.5]
6. `-s`: Probability of timeout; type:`float`; range: (0, 1)
7. `-T`: Total number of updates;  type: `int` 
8. `-o`: Output file name;  type: `string`

#### Examples
1. To plot graphs for all parameter combinations
```
$ python3 script.py -a
```

2. To run for specific paramter values
```
$ python3 script.py -i 1 -m 1 -n 0.5 -f 0.1 -s 0.01 -T 1000 -o cw_values
```


### Running the simulator
The simulator takes different parameter values as command line arguments and outputs a file with congestion window size after each update. The command line arguments are the same as the arguments 2-8 of the script.

To run the script, we first compile the simulator.
```
$ make
```

Next, we invoke the simulator with the parameter values. An example is shown below.
```
$ ./cw -i 1 -m 1 -n 0.5 -f 0.1 -s 0.01 -T 1000 -o cw_values
```

### Clean-up
To remove the binary files, run the following command.
```
$ make clean
```