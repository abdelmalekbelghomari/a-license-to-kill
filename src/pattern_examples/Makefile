CC=gcc
CPPFLAGS1=-I. -I./facade
CPPFLAGS2=-I. -I./factory
CPPFLAGS3=-I. -I./observer
CPPFLAGS4=-I. -I./state
CFLAGS=-Wall -Wextra -pedantic -g
EXEC=facade.bin factory.bin observer.bin state_character.bin

.PHONY: all clean

all: $(EXEC)

singleton.bin: singleton.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@

facade.bin: facade/main.c facade/facade.c facade/memory.c logger.c
	$(CC) $(CPPFLAGS1) $(CFLAGS) $^ -o $@

factory.bin: factory/main.c factory/factory.c logger.c
	$(CC) $(CPPFLAGS2) $(CFLAGS) $^ -o $@

observer.bin: observer/memory.c observer/observer.c logger.c observer/main.c
	$(CC) $(CPPFLAGS3) $(CFLAGS) $^ -o $@

state_character.bin: state/main.c state/character.c logger.c
	$(CC) $(CPPFLAGS4) $(CFLAGS) $^ -o $@

clean:
	rm $(EXEC)
