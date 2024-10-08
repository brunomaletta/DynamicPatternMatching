#include <iostream>
#include <string>

#include "../../src/alg/sa.cpp"
#include "../../src/aux/timer.cpp"

int main(int argc, char** argv) {
	srand(atoi(argv[1]));

	int n;
	std::cin >> n;

	int q;
	std::cin >> q;
	int half = q/2;

	std::string s;
	for (int i = 0; i < n; i++) s += 'a'+rand()%2;

	suffix_array sa(s);
	std::string p;
	int sum = 0;
	timer T;

	while (q > 0) {
		int o = rand()%3+1;
		if (q > half and o == 2) o = 1;
		if (o == 1) { // add char
			int idx = rand()%(p.size()+1);
			char c = 'a'+rand()%2;
			p.insert(p.begin() + idx, c);
		} else if (o == 2) { // delete char
			if (p.size() == 0) continue;
			p.erase(p.begin() + rand()%p.size());
		} else if (o == 3) {
			auto [l, r, lcp] = sa.pattern_search(p);
			sum += r-l;
		}
		q--;
	}

	int time = T();

	std::cout << "naive" << std::endl;
	std::cout << sum << std::endl;
	std::cout << time << std::endl;

	return 0;
}
