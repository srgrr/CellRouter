CXX=g++
COMPILER_FLAGS=-std=c++11 -O2 -Wall
LIBS=-lpblib -lm
FLAGS=${COMPILER_FLAGS} ${LIBS}

%.o: %.cc %.h
	${CXX} -c -o $@ $< ${FLAGS}

# Common linkable stuff, as utils and so on
COMMON_OBJECTS=emst.o instance.o net.o abstract_formula.o abstract_constraint.o

%.exe: %.cc ${COMMON_OBJECTS}
	${CXX} -o $@ $< ${COMMON_OBJECTS} ${FLAGS}

all: router.exe

clean:
	rm -f *.o *.exe
