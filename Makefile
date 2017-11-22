COMPILER_FLAGS=-std=c++11 -O2 -Wall
LIBS=-lpblib
FLAGS=${COMPILER_FLAGS} ${LIBS}

# OBJECTS
utils.o: utils.cc
	g++ -c utils.cc ${FLAGS}

instance.o: instance.cc
	g++ -c instance.cc ${FLAGS}

# input -> problem instance -> CNF formula

# Common linkable stuff, as utils and so on
COMMON_OBJECTS=utils.o



# MAIN PROGRAMS
prova: prova.cc ${COMMON_OBJECTS} instance.o
	g++ -o prova prova.cc ${COMMON_OBJECTS} instance.o ${FLAGS}

router: router.cc ${COMMON_OBJECTS} instance.o
	g++ -o router router.cc ${COMMON_OBJECTS} instance.o ${FLAGS}

plot_rl_algo: plot_rl_algo.cc ${COMMON_OBJECTS} instance.o
	g++ -o plot_rl_algo plot_rl_algo.cc ${COMMON_OBJECTS} instance.o ${FLAGS}



clean:
	rm *.o
