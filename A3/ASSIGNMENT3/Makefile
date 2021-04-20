# Name: Anirudh S
# Roll no: CS17B003

CC := g++
CFLAGS := -std=c++11 -pthread

SRCS := $(wildcard *.cc)
PROGS := ospf

all : ospf

ospf: $(SRCS)
	$(CC) $(CFLAGS) -o ospf $(SRCS)

clean :
	-rm $(PROGS)
