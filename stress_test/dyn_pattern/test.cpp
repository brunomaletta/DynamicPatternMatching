#include <iostream>
#include <string>

#include "../../src/alg/matching.cpp"

int main() {
	std::string t;
	std::cin >> t;
	dyn_pattern::matching m(t);
	int q;
	std::cin >> q;
	while (q--) {
		int o;
		std::cin >> o;
		if (o == 1) {
			int i;
			char c;
			std::cin >> i >> c;
			m.insert(i-1, c);
		} else if (o == 2) {
			int i;
			std::cin >> i;
			m.erase(i-1);
		} else if (o == 3) {
			std::cout << m.matches() << std::endl;
		}
	}
	return 0;
}

