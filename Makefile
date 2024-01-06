CC=gcc
CFLAGS=-Wall -Wextra -pedantic -O2 -g -DMUTEX

# Compilation under MacOS X or Linux
UNAME=$(shell uname -s)
ifeq ($(UNAME),Darwin)
    CPPFLAGS=-D_REENTRANT -I./include -I/usr/local/Cellar/ncurses/6.3/include
    LDFLAGS=-L/usr/local/Cellar/ncurses/6.3/lib -lncurses -lpthread -g -lm
endif
ifeq ($(UNAME),Linux)
    CPPFLAGS=-D_REENTRANT -I./include
    LDFLAGS=-lncurses -lpthread -g -lrt -lm
endif

.PHONY: all clean distclean

all: bin/spy_simulation bin/monitor bin/timer bin/citizen_manager bin/enemy_spy_network bin/enemy_country bin/counter_intelligence

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
bin/monitor: src/monitor/main.o src/monitor/monitor.o src/monitor/monitor_common.o src/common/logger.o 
	$(CC) $^ -o $@ $(LDFLAGS)

src/monitor/main.o: src/monitor/main.c include/monitor.h include/monitor_common.h include/timer.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

src/monitor/monitor.o: src/monitor/monitor.c include/monitor.h include/timer.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# ----------------------------------------------------------------------------
# TIMER
# ----------------------------------------------------------------------------
bin/timer: src/timer/main.o 
	$(CC) $^ -o $@ $(LDFLAGS)

src/timer/main.o: src/timer/main.c include/timer.h include/memory.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $< -o $@ -c

# ----------------------------------------------------------------------------
# CITIZEN MANAGER
# ----------------------------------------------------------------------------
bin/citizen_manager: src/citizen_manager/main.o\
					 src/citizen_manager/citizen_manager.o\
					 src/astar/astar.o
	$(CC) $^ -o $@ $(LDFLAGS)

src/citizen_manager/main.o: src/citizen_manager/main.c\
							include/memory.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

src/citizen_manager/citizen_manager.o:  src/citizen_manager/citizen_manager.c \
										include/spy_simulation.h \
										include/memory.h \
										include/citizen_manager.h \
										include/astar.h \
										include/monitor_common.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

src/astar/astar.o:  src/astar/astar.c \
					include/astar.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# ----------------------------------------------------------------------------
# ENEMY SPY NETWORK
# ----------------------------------------------------------------------------
bin/enemy_spy_network:	src/enemy_spy_network/main.o\
						src/enemy_spy_network/enemy_spy_network.o\
						src/astar/astar.o 
	$(CC) $^ -o $@ $(LDFLAGS)

src/enemy_spy_network/main.o: 	src/enemy_spy_network/main.c\
								include/enemy_spy_network.h\
							  	include/memory.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

src/enemy_spy_network/enemy_spy_network.o:	src/enemy_spy_network/enemy_spy_network.c\
											include/enemy_spy_network.h\
											include/astar.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# ----------------------------------------------------------------------------
# ENEMY COUNTRY
# ----------------------------------------------------------------------------
bin/enemy_country: src/enemy_country/main.o
	$(CC) $^ -o $@ $(LDFLAGS)

src/enemy_country/main.o: src/enemy_country/main.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# ----------------------------------------------------------------------------
# COUNTER INTELLIGENCE
# ----------------------------------------------------------------------------
bin/counter_intelligence: src/counter_intelligence/main.o src/counter_intelligence/counter_intelligence.o src/astar/astar.o
	$(CC) $^ -o $@ $(LDFLAGS)

src/counter_intelligence/main.o: src/counter_intelligence/main.c include/counter_intelligence.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

src/counter_intelligence/counter_intelligence.o: src/counter_intelligence/counter_intelligence.c include/counter_intelligence.h include/astar.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# ----------------------------------------------------------------------------
# CLEANING
# ----------------------------------------------------------------------------
clean:
	rm -f src/spy_simulation/*.o src/monitor/*.o src/common/*.o src/timer/*.o src/citizen_manager/*.o src/enemy_spy_network/*.o src/counter_intelligence/*.o

distclean: clean
	rm -f bin/*
