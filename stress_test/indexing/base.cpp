#include <iostream>
#include <string>
#include <vector>

int matching(std::string& t, std::string& p) {
	if (!p.size()) return 0;
	std::vector<int> pi(p.size());
	int j = 0;
	for (int i = 1; i < (int) p.size(); i++) {
		while (j > 0 and p[i] != p[j]) j = pi[j-1];
		if (p[i] == p[j]) j++;
		pi[i] = j;
	}
	j = 0;
	int occ = 0;
	for (int i = 0; i < (int) t.size(); i++) {
		while (j > 0 and t[i] != p[j]) j = pi[j-1];
		if (t[i] == p[j]) j++;
		if (j == (int) p.size()) {
			occ++;
			j = pi[j-1];
		}
	}
	return occ;
}

int main() {
	std::string t;
	std::cin >> t;
	int q;
	std::cin >> q;
	while (q--) {
		std::string p;
		std::cin >> p;
		int m = matching(t, p);
		std::cout << m << " ";
		if (m > 0) std::cout << p.size() << std::endl;
		else {
			std::string pp(1, p[0]);
			while (matching(t, pp)) pp += p[pp.size()];
			std::cout << pp.size()-1 << std::endl;
		}
	}
	return 0;
}

