#include <iostream>
#include <string>

int main(int argc, char** argv) {
	int CNT = atoi(argv[1]);
	std::string s;
	int s1 = 0, s2 = 0, mem = 0;
	for (int i = 0; i < CNT; i++) {
		std::cin >> s;
		int a, x, m;
		std::cin >> x >> a >> m;
		s1 += a;
		s2 += x;
		mem += m;
	}
	std::cout << s << std::endl;
	//std::cout << s2 << std::endl;
	std::cout << s1/CNT << " ms" << std::endl;
	std::cout << mem/CNT << " MB" << std::endl;
	return 0;
}

