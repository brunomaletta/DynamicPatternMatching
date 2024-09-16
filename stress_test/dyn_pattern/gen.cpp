#include <iostream>

const int LIM = 1e4;

int main(int argc, char** argv) {
	srand(atoi(argv[1]));
	int n = rand()%LIM+1;
	for (int i = 0; i < n; i++) std::cout << char('a'+rand()%2);
	std::cout << std::endl;
	int q = rand()%LIM+1;
	std::cout << q << std::endl;
	int m = 0;
	while (q > 0) {
		int o = rand()%3+1;
		if (o == 1) { // add char
			std::cout << o << " " << rand()%(m+1)+1 << " " << char('a'+rand()%2) << std::endl;
			m++;
		} else if (o == 2) { // delete char
			if (m == 0) continue;
			std::cout << o << " " << rand()%m+1 << std::endl;
			m--;
		} else if (o == 3) { // query
			std::cout << o << std::endl;
		}
		q--;
	}
	return 0;
}

