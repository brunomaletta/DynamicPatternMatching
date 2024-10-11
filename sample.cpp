#include <iostream>

#include "src/alg/matching.cpp"

int main() {
	dyn_pattern::matching m("aabaacaadaabaaba");
	m.insert(0, "aaba");
	for (int i = 0; i < m(); i++)
		std::cout << m[i] << std::endl;
	m.erase(2);
	std::cout << m.matches() << std::endl;
}

