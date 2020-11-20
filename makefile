CC=gcc
CFLAGS= -c -Wall -gdwarf-2 -std=c99
DEPS = trader_bot.h Utilities.h

OBJ = Utilities.o bot.o Tester1.o print_world.o WorldAnalyzers.o Scorers.o MultiBot.o

all: run_bot

run_bot: $(OBJ)
	$(CC)  $(OBJ) -o run_bot -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ 

.PHONY : all clean

clean:
	rm -f *.o run_bot