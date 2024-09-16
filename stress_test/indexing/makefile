FLAGS = -std=c++17 -O2
DEBUG_FLAGS = -fsanitize=address,undefined -fno-omit-frame-pointer -g -Wall -Wshadow

all: base test gen
	bash ./stress.sh base test gen

debug: base_db test_db gen_db
	bash ./stress.sh base_db test_db gen_db
	
base: base.cpp
	g++ -o base base.cpp $(FLAGS)

base_db: base.cpp
	g++ -o base_db base.cpp $(FLAGS) $(DEBUG_FLAGS)
	
test: test.cpp
	g++ -o test test.cpp $(FLAGS)

test_db: test.cpp
	g++ -o test_db test.cpp $(FLAGS) $(DEBUG_FLAGS)
	
gen: gen.cpp
	g++ -o gen gen.cpp $(FLAGS)

gen_db: gen.cpp
	g++ -o gen_db gen.cpp $(FLAGS) $(DEBUG_FLAGS)

clean:
	rm -f out_base.txt out_test.txt in base test gen base_db test_db gen_db
