CXX=g++
COMPILER_FLAGS=-std=c++11 -O2 -Wall
LIBS=-lpblib
FLAGS=${COMPILER_FLAGS} ${LIBS}

%.o: %.cc
	${CXX} -c -o $@ $< ${FLAGS}

# Common linkable stuff, as utils and so on
COMMON_OBJECTS=utils.o instance.o

%.exe: %.cc ${COMMON_OBJECTS}
	${CXX} -o $@ $< ${COMMON_OBJECTS} ${FLAGS}

all: prova.exe router.exe plot_rl_algo.exe

clean:
	rm -f *.o *.exe
