# Name: Anirudh S
# Roll no: CS17B003

CC := gcc
CFLAGS := -Wall

SRCS := $(wildcard *.c)
PROGS := $(patsubst %.c,%,$(SRCS))

all : $(PROGS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean :
	-rm $(PROGS)