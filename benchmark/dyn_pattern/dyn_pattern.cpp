#include <iostream>
#include <string>

#include "../../src/alg/matching.cpp"
#include "../../src/aux/timer.cpp"

int main(int argc, char** argv) {
	srand(atoi(argv[1]));

	int n;
	std::cin >> n;

	int q;
	std::cin >> q;

	std::string s;
	for (int i = 0; i < n; i++) s += 'a'+rand()%2;

	dyn_pattern::matching m(s);
	int sz = 0;
	int sum = 0;
	timer T;

	while (q > 0) {
		int o = rand()%3+1;
		if (o == 1) { // add char
			int idx = rand()%(sz+1);
			char c = 'a'+rand()%2;
			m.insert(idx, c);
			sz++;
		} else if (o == 2) { // delete char
			if (sz == 0) continue;
			m.erase(rand()%sz);
			sz--;
		} else if (o == 3) {
			sum += m.matches();
		}
		q--;
	}

	int time = T();

	std::cout << "dyn_array" << std::endl;
	std::cout << sum << std::endl;
	std::cout << time << std::endl;

	return 0;
}
