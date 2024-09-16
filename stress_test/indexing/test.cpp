#include <iostream>
#include <string>

#include "../../src/alg/sa.cpp"

int main() {
	std::string t;
	std::cin >> t;
	suffix_array sa(t);
	int q;
	std::cin >> q;
	while (q--) {
		std::string p;
		std::cin >> p;
		auto [l, r, pattern_lcp] = sa.pattern_search(p);
		std::cout << r-l << " " << pattern_lcp << std::endl;
	}
	return 0;
}

