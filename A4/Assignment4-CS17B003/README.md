<!--
Name: Anirudh S
Roll No: CS17B003
-->

# CS3205 Assignment 4

## Compilation
To compile the code, run
```
$ make
```
All the four executables are generated.

## Selective Repeat

### Sender
SenderSR accepts the following flags:
* -­d – Turn ON Debug Mode (OFF if ­d flag not present)
* -­s string – Receiver Name or IP address.
* ­-p integer – Receiver’s Port Number
* ­-n integer – Sequence Number Field Length (in bits)
* ­-L integer – MAX_PACKET_LENGTH, in bytes
* ­-R integer – PACKET_GEN_RATE, in packets per second
* ­-N integer – MAX_PACKETS
* ­-W integer – WINDOW_SIZE (Assume that SWS = RWS)
* ­-B integer – BUFFER_SIZE

Example command to run SenderSR
```
java SenderSR -d -s 127.0.0.1 ­-p 12345 -N 100 -W 4 -B 20 -n 5 -R 20 -L 256
```

### Receiver
ReceiverSR accepts the following flags:
* -d – Turn ON Debug Mode (OFF if ­d flag not present)
* -p integer – Receiver’s Port Number
* -­N integer – MAX_PACKETS
* -n integer – Sequence Number Field Length (in bits)
* -W integer – WINDOW_SIZE (SWS = RWS)
* -B integer – BUFFER_SIZE
* -e double – PACKET_ERROR_RATE
Example command to run ReceiverSR
```
java ReceiverSR -d -p 12345 -N 100 -n 5 -B 25 -W 4 -e 0.1
```

## Go-Back-N

### Sender
SenderGBN accepts the following flags:
* -d – Turn ON Debug Mode (OFF if ­d flag not present)
* -s string – Receiver Name or IP address.
* -p integer – Receiver’s Port Number
* -l integer – PACKET_LENGTH, in bytes
* -r integer – PACKET_GEN_RATE, in packets per second
* -n integer – MAX_PACKETS
* -­w integer – WINDOW_SIZE
* -b integer – MAX_BUFFER_SIZE
Example command to run SenderGBN
```
java SenderGBN -d -s 127.0.0.1 -p 12345 -n 100 -w 4 -b 20 -r 20 -l 256 
```

### Receiver
ReceiverGBN accepts the following flags:
* -d – Turn ON Debug Mode (OFF if ­d flag not present)
* -p integer – Receiver’s Port Number
* -n integer – MAX_PACKETS
* -e float – Packet Error Rate (RANDOM_DROP_PROB)
Example command to run ReceiverGBN
```
java ReceiverGBN -d -p 12345 -n 100 -e 0.05
```
