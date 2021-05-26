<!--
Name: Anirudh S
Roll no: CS17B003
-->

# Assignment 1: Mail client server application

## Instructions to run the code
Please follow the instructions given below to compile and execute the code. All commands given here are `bash` commands.

## Compilation
To compile, run the Makefile using the command
```
$ make
```

## Server side
To start the server, execute the binary and provide the port number it should use as a command line argument.
```
$ ./emailserver <port-no> &
```

For example,
```
$ ./emailserver 8080 &
```

Port number 8080 was used while testing. Use other port numbers only after checking if they are safe.
`&` added at the end is to make the code run in the background.


## Client side
Start the client only after starting the server. To start the client, execute the binary along with the local IP address and port number
of the server as command line arguments.
```
$ ./emailclient <IP-addr> <port-no>
```

For example (using localhost and the server set up in the above example),
```
$ ./emailclient 127.0.0.1 8080
```

This will open up the prompt using which we can communicate with the server. For the list of commands, refer the assignment document.

NOTE: Local/Priavte IP address of the server machine can be found out by following the steps provided in the following links.
1. Linux: https://linuxize.com/post/how-to-find-ip-address-linux/
2. Windows/Mac: https://www.avg.com/en/signal/find-ip-address


## Clean-up
Run the following command.
```
$ make clean
```

This will remove the binary files.
The mail spool files, present in `MAILSERVER` directory, created by the server are left untouched so that they can be inspected later.
