CC = g++
CPP = $(CC)

SCHED_OBJS = main.o scheduler.o job.o

COMMON_LIBS = -lSDL -lBox2D

CFLAGS += -std=c++11 -Wall -Weffc++ -pedantic -U__STRICT_ANSI__ -g 

default: sched

all: sched

sched: $(SCHED_OBJS)
	$(CPP) -g -o sched $(SCHED_OBJS)

%.o: %.cpp
	$(CPP) -c -o $@ $(CFLAGS) $(INCLUDES) $<

clean:
	$(RM) *.o $(EXE) core

