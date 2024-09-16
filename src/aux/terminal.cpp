#pragma once

#include <unistd.h>
#include <termios.h>

namespace terminal {

void reset_termios() {
	struct termios old = {0};
	if (tcgetattr(0, &old) < 0)
		perror("tcsetattr()");
	old.c_lflag |= (ICANON | ECHO);
	if (tcsetattr(0, TCSADRAIN, &old) < 0)
		perror("tcsetattr ~ICANON");
}

char getch(int timer_ds = 0) {
	char buf = 0;
	struct termios old = {0};
	if (tcgetattr(0, &old) < 0)
		perror("tcsetattr()");
	old.c_lflag &= ~(ICANON | ECHO);
	old.c_cc[VMIN] = timer_ds == 0;
	old.c_cc[VTIME] = timer_ds;
	if (tcsetattr(0, TCSANOW, &old) < 0)
		perror("tcsetattr ICANON");
	if (read(0, &buf, 1) < 0)
		perror ("read()");
	old.c_lflag |= (ICANON | ECHO);
	if (tcsetattr(0, TCSADRAIN, &old) < 0)
		perror("tcsetattr ~ICANON");
	return buf;
}

char getch_now(int timer_ds = 0) {
	char buf = 0;
	struct termios old = {0};
	if (tcgetattr(0, &old) < 0)
		perror("tcsetattr()");
	old.c_lflag &= ~(ICANON | ECHO);
	old.c_cc[VMIN] = 0;
	old.c_cc[VTIME] = timer_ds;
	if (tcsetattr(0, TCSANOW, &old) < 0)
		perror("tcsetattr ICANON");
	int qt_read = read(0, &buf, 1);
	if (qt_read < 0)
		perror ("read()");
	old.c_lflag |= (ICANON | ECHO);
	if (tcsetattr(0, TCSADRAIN, &old) < 0)
		perror("tcsetattr ~ICANON");
	if (qt_read == 0) return 0;
	return buf;
}

void new_screen() {
	std::cout << "\0337\033[?47h";
	std::cout.flush();
}

void close_screen() {
	std::cout << "\033[2J\033[?47l\0338";
	std::cout.flush();
}

void clear_screen() {
	std::cout << "\x1B[2J\x1B[H";
	std::cout.flush();
}

void hide_cursor() {
	std::cout << "\e[?25l";
	std::cout.flush();
}

void enable_cursor() {
	std::cout << "\e[?25h";
	std::cout.flush();
}

std::string cursor() {
	return "\033[s";
}

void print_cursor() {
	std::cout << " \033[u";
	std::cout.flush();
}

std::ostream& bold_on(std::ostream& os) {
	return os << "\e[1m";
}

std::ostream& underline_on(std::ostream& os) {
	return os << "\033[4m";
}

std::ostream& invert_on(std::ostream& os) {
	return os << "\033[7m";
}

std::ostream& reset_format(std::ostream& os) {
	return os << "\e[0m";
}

}; // namespace terminal

#define BOLD(P) terminal::bold_on << P << terminal::reset_format
#define INVERT(P) terminal::invert_on << P << terminal::reset_format
#define UNDERLINE(P) terminal::underline_on << P << terminal::reset_format

