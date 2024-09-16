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
	std::string p;
	while (q--) {
		int o;
		std::cin >> o;
		if (o == 1) {
			int i;
			char c;
			std::cin >> i >> c;
			p.insert(p.begin() + i-1, c);
		} else if (o == 2) {
			int i;
			std::cin >> i;
			p.erase(p.begin() + i-1);
		} else if (o == 3) {
			std::cout << matching(t, p) << std::endl;
		}
	}
	return 0;
}

