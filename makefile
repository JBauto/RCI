CC = gcc
CFLAGS = -Wall -g

all: ddt testTejo

# ddt
ddt: ddt.o interface.o network.o
	$(CC) $(CFLAGS) $^ -o $@

ddt.o: ddt.c interface.h network.h
	$(CC) $(CFLAGS) -c $< -o $@

interface.o: interface.c
	$(CC) $(CFLAGS) -c $< -o $@

network.o: network.c
	$(CC) $(CFLAGS) -c $< -o $@

#testTejo
testTejo: testTejo.o
	$(CC) $(CFLAGS) $^ -o $@

testTejo.o: testTejo.c
	$(CC) $(CFLAGS) -c $< -o $@

#clean
clean:
	clear
	rm *.o
	rm ddt
	rm testTejo


# $@ reffers to named before :
# $^ reffers to all named after :
# $< reffers to first after :
