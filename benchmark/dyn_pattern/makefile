FLAGS = -std=c++17 -O2

all: average dyn_pattern naive
	bash ./run.sh

average: average.cpp
	g++ -o average average.cpp $(FLAGS)
	
dyn_pattern: dyn_pattern.cpp
	g++ -o dyn_pattern dyn_pattern.cpp $(FLAGS)

naive: naive.cpp
	g++ -o naive naive.cpp $(FLAGS)

clean:
	rm -f input output average dyn_pattern naive
