FLAGS = -std=c++17 -O2

all: pi

pi: main
	./main text/pi2-ten-million.txt

pi2: main
	./main text/pi3-100-million.txt

bible: main
	./main text/bible.txt
	
main: main.cpp
	g++ -o main main.cpp $(FLAGS)

clean:
	rm main
