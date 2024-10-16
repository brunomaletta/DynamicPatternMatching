FLAGS = -std=c++17 -O2

all: pi

pi: main
	./main text/pi2-ten-million.txt

pi2: main
	./main text/pi3-100-million.txt

bible: main
	./main text/bible.txt

casmurro: main
	./main text/casmurro.txt
	
main: main.cpp src/alg/matching.cpp src/alg/sa.cpp src/alg/treap.cpp src/alg/rmq.cpp src/aux/terminal.cpp src/aux/timer.cpp
	g++ -o main main.cpp $(FLAGS)

example: sample
	./sample

sample: sample.cpp src/alg/matching.cpp src/alg/sa.cpp src/alg/treap.cpp src/alg/rmq.cpp
	g++ -o sample sample.cpp $(FLAGS)

clean:
	rm -rf main sample
