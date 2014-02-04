CC = g++
CPP = $(CC)

SCHED_OBJS = main.o scheduler.o

ifdef SystemRoot
  RM = del /Q /S
  EXE = sched.exe
else
  RM = rm -rf
  EXE = sched
endif

CFLAGS += -std=c++11 -Wall -Weffc++ -pedantic -U__STRICT_ANSI__ -g 

default: sched

all: sched

sched: $(SCHED_OBJS)
	$(CPP) -g -o sched $(SCHED_OBJS)

%.o: %.cpp
	$(CPP) -c -o $@ $(CFLAGS) $(INCLUDES) $<

clean:
	$(RM) *.o $(EXE) core

