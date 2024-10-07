FLAGS = -std=c++17 -O2

all: average dyn_pattern
	bash ./run.sh

average: average.cpp
	g++ -o average average.cpp $(FLAGS)
	
dyn_pattern: dyn_pattern.cpp
	g++ -o dyn_pattern dyn_pattern.cpp $(FLAGS)

clean:
	rm -f input output average dyn_pattern
