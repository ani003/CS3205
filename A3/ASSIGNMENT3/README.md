# Assignment 2: TCP congestion control

## Compilation
To compile the code, run
```
$ make
```
The binary `ospf` will be generated.

## Running the program
The program accepts the following flags:
1. `-i`: ID of the router; type: `int`
2. `-f`: Input file name; type: `string`
3. `-o`: Output file name; type: `string`
4. `-h`: Hello interval; type(in seconds): `float`; default: 1.0
5. `-a`: LSA interval(in seconds); type:`float`; default: 5.0.
6. `-s`: SPF interval(in seconds); type:`float`; default: 20.0.

To run the file, open 8 terminals(T0-7). In terminal i, type the following
```
$ ./ospf -i <i> -f input.txt -o outfile<i>.txt -h 1 -a 5 -s 20
```

For example, in terminal 0,
```
$ ./ospf -i 0 -f input.txt -o outfile0.txt -h 1 -a 5 -s 20
```