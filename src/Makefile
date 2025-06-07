#
# Simple Makefile for compiling programs for initio robot car 
#
SHELL	= bash
GCC	= gcc
CFLAGS	= -Wall -Werror -mfloat-abi=hard
LFLAGS	= -linitio -lcurses -lwiringPi -lpthread

PROG 	= parSens

.PHONY: all run help

all: $(PROG)

run: $(PROG)
	./$<

schedule: $(PROG)
	rtcs_schedule $<

% : %.c
	$(GCC) -o $@ $(CFLAGS) $(LFLAGS) $<

clean:
	rm -r $(PROG)

help:
	@echo
	@echo "Possible commands:"
	@echo " > make run"
	@echo " > make schedule"
	@echo " > make clean"
	@echo

