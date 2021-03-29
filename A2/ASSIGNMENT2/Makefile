# Name: Anirudh S
# Roll no: CS17B003

CC := g++
CFLAGS := -Wall

SRCS := $(wildcard *.cc)
PROGS := $(patsubst %.cc,%,$(SRCS))

all : $(PROGS)

%: %.cc
	$(CC) $(CFLAGS) -o $@ $<

clean :
	-rm $(PROGS)
