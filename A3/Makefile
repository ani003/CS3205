# Name: Anirudh S
# Roll no: CS17B003

CC := g++
CFLAGS := -Wall -std=c++11 -pthread

SRCS := $(wildcard *.cc)
PROGS := $(patsubst %.cc,%,$(SRCS))

all : $(PROGS)

%: %.cc
	$(CC) $(CFLAGS) -o $@ $<

clean :
	-rm $(PROGS)
