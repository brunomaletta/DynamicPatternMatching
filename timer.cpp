#pragma once

#include <chrono>

struct timer : std::chrono::high_resolution_clock {
	const time_point start;
	timer(): start(now()) {}
	int operator()() { // integer ms time
		return std::chrono::duration_cast<std::chrono::milliseconds>(now() - start).count();
	}
	double operator*() { // double ms time
		return std::chrono::duration<double, std::milli>(now() - start).count();
	}
};

