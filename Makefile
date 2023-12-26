CC=gcc
CFLAGS=-Wall -Wextra -pedantic -O2 -g

# Compilation under MacOS X or Linux
UNAME=$(shell uname -s)
ifeq ($(UNAME),Darwin)
    CPPFLAGS=-D_REENTRANT -I./include -I/usr/local/Cellar/ncurses/6.3/include
    LDFLAGS=-L/usr/local/Cellar/ncurses/6.3/lib -lncurses -lpthread -g -lm
endif
ifeq ($(UNAME),Linux)
    CPPFLAGS=-D_REENTRANT -I./include
    LDFLAGS=-lncurses -lpthread -lrt -lm
endif

.PHONY: all clean distclean

all: bin/spy_simulation bin/monitor bin/timer

# ----------------------------------------------------------------------------
# SPY SIMULATION
# ----------------------------------------------------------------------------
bin/spy_simulation: src/spy_simulation/main.o src/spy_simulation/spy_simulation.o 
	$(CC) $^ -o $@ $(LDFLAGS)

src/spy_simulation/main.o: src/spy_simulation/main.c include/spy_simulation.h include/memory.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

src/spy_simulation/spy_simulation.o: src/spy_simulation/spy_simulation.c include/spy_simulation.h include/memory.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# ----------------------------------------------------------------------------
# MONITOR
# ----------------------------------------------------------------------------
bin/monitor: src/monitor/main.o src/monitor/monitor.o src/monitor/monitor_common.o src/common/logger.o src/timer/timer.o
	$(CC) $^ -o $@ $(LDFLAGS)

src/monitor/main.o: src/monitor/main.c include/monitor.h include/monitor_common.h include/timer.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

src/monitor/monitor.o: src/monitor/monitor.c include/monitor.h include/timer.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# ----------------------------------------------------------------------------
# TIMER
# ----------------------------------------------------------------------------
bin/timer: src/timer/main.o src/timer/timer.o 
	$(CC) $^ -o $@ $(LDFLAGS)

src/timer/main.o: src/timer/main.c src/timer/timer.c include/timer.h include/memory.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ -c

src/timer/timer.o : src/timer/timer.c include/timer.h include/memory.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ -c


# ----------------------------------------------------------------------------
# CLEANING
# ----------------------------------------------------------------------------
clean:
	rm -f src/spy_simulation/*.o src/monitor/*.o src/common/*.o src/timer/*.o

distclean: clean
	rm -f bin/spy_simulation bin/monitor bin/timer
