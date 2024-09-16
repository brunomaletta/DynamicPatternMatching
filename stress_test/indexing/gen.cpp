#include <iostream>

const int LIM_T = 1e5;
const int LIM_P = 1e2;
const int LIM_Q = 1e2;

int main(int argc, char** argv) {
	srand(atoi(argv[1]));
	int n = rand()%LIM_T+1;
	for (int i = 0; i < n; i++) std::cout << char('a'+rand()%2);
	std::cout << std::endl;
	int q = rand()%LIM_Q+1;
	std::cout << q << std::endl;
	while (q--) {
		int m = rand()%LIM_P+1;
		for (int i = 0; i < m; i++) std::cout << char('a'+rand()%2);
		std::cout << std::endl;
	}
	return 0;
}

