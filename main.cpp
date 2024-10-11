#include <iostream>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <thread>
#include <atomic>
#include <mutex>
#include <csignal>
#include <sstream>

#include "src/alg/matching.cpp"
#include "src/aux/terminal.cpp"
#include "src/aux/timer.cpp"

const int MAX_OC_PRINT = 10;
const int MAX_OC_RANGE = 20;

bool valid_char(char c) {
	if (c == ' ') return true;
	if (c >= 'A' and c <= 'Z') return true;
	if (c >= 'a' and c <= 'z') return true;
	if (c >= '0' and c <= '9') return true;
	return false;
}

void format(std::string& t) {
	for (char& c : t) {
		if (c == '\n') c = ' ';
		if (c >= 'A' and c <= 'Z') c ^= 32;
		if (!valid_char(c)) c = ' ';
	}
	std::string t2;
	for (char c : t) {
		if (c == ' ') {
			if (t2.size() and t2.back() == c) continue;
		}
		t2 += c;
	}
	t = t2;
}

std::string format_int(int n) {
	std::string ret;
	int cnt = 0;
	while (n > 0) {
		if (cnt > 0 and cnt % 3 == 0) ret += ' ';
		cnt++;
		ret += '0' + (n % 10);
		n /= 10;
	}
	std::reverse(ret.begin(), ret.end());
	if (ret.size() == 0) ret = "0";
	return ret;
}

std::string format_size(int n) {
	if (n < 1024) return std::to_string(n) + " B";
	n /= 1024;
	if (n < 1024) return std::to_string(n) + " KB";
	n /= 1024;
	if (n < 1024) return std::to_string(n) + " MB";
	n /= 1024;
	return std::to_string(n) + " GB";
}

std::mutex print_mutex;

void handle_exit_signal() {
	std::lock_guard<std::mutex> guard(print_mutex);
	terminal::close_screen();
	exit(1);
}

void interrupt_handler(int s){
	handle_exit_signal();
}

void terminal_stop_handler(int s){
	std::lock_guard<std::mutex> guard(print_mutex);
	terminal::close_screen();
}

void continue_handler(int s) {
	std::lock_guard<std::mutex> guard(print_mutex);
	terminal::reset_termios();
	terminal::new_screen();
	terminal::reset_termios();
}

void setup() {
	signal(SIGINT, interrupt_handler);
	signal(SIGTSTP, terminal_stop_handler);
	signal(SIGCONT, continue_handler);
}

void look_for_interruption(std::atomic<bool>& stop_condition) {
	while (!stop_condition) {
		char c = terminal::getch_now();
		if (c) handle_exit_signal();
	}
}

void print_loading(std::string op, std::atomic<bool>& stop_condition) {
	std::thread interrupt_thread(look_for_interruption, std::ref(stop_condition));
	timer T;
	int cnt = 0;
	while (!stop_condition) {
		{
			std::lock_guard<std::mutex> guard(print_mutex);
			terminal::clear_screen();
			std::cout << op << std::endl;

			std::cout << std::endl << "Elapsed time: " << BOLD(T()/1000) << " seconds";
			for (int i = 0; i < std::min(3, cnt/2); i++) std::cout << ".";
			std::cout << std::endl;
			if (++cnt == 20) cnt = 0;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	interrupt_thread.join();
}

void read_text_thread_func(std::ifstream& in, std::string text_name, std::string& t,
	std::atomic<bool>& read_completed) {
	std::string line;
	while (getline(in, line)) {
		t += line;
		t += '\n';
	}
	in.close();
	format(t);
	read_completed = true;
}

std::string read_text(std::ifstream& in, std::string text_name) {
	std::atomic<bool> read_completed = false;
	std::string t;

	std::thread read_text_thread(read_text_thread_func, std::ref(in), text_name, std::ref(t),
		std::ref(read_completed));

	print_loading("Reading text " + text_name, read_completed);
	read_text_thread.join();

	return t;
}

void compute_sa_thread_func(dyn_pattern::matching& m, std::string& t,
	std::atomic<bool>& sa_completed, int& build_time_ms) {
	timer T;
	m = dyn_pattern::matching(t);
	build_time_ms = T();
	sa_completed = true;
}

int preprocess(dyn_pattern::matching& m, std::string& text_name, std::string& t) {
	int build_time_ms = 0;
	std::atomic<bool> sa_completed = false;

	std::thread compute_sa_thread(compute_sa_thread_func, std::ref(m), std::ref(t),
		std::ref(sa_completed), std::ref(build_time_ms));

	std::stringstream ss;
	ss << "Building Suffix Array for text " << text_name << " with "
		<< BOLD(format_int(t.size())) << " characters (" << format_size(t.size()) << ")"
		<< std::endl;
	print_loading(ss.str(), sa_completed);
	compute_sa_thread.join();

	return build_time_ms;
}

void run_matching(dyn_pattern::matching& m, std::string& text_name, std::string& t,
	int build_time_ms) {
	std::string p;
	int cursor_pos = 0;
	int occ_shift = 0;
	double update_time_ms = 0;
	terminal::enable_cursor();
	while (1) {
		terminal::clear_screen();
		std::cout << "Build time: " << BOLD(format_int(build_time_ms) << " ms") << " for text "
			<< text_name << " with " << BOLD(format_int(t.size())) << " characters ("
			<< format_size(t.size()) << ")" << std::endl;
		std::cout << std::endl << '\t' << "--> ";
		for (int i = 0; i < cursor_pos; i++) std::cout << p[i];
		std::cout << terminal::cursor();
		for (int i = 0; i < p.size() - cursor_pos; i++) std::cout << p[cursor_pos+i];
		std::cout << std::endl << std::endl;
		std::cout << "Number of occurences: " << UNDERLINE(BOLD(format_int(m.matches())))
			<< std::endl;
		std::cout << "Time taken to update matches: " << std::setprecision(1)
			<< UNDERLINE(BOLD(update_time_ms << " ms")) << std::endl;

		int qt_oc = std::min(MAX_OC_PRINT, m.matches());
		if (qt_oc > 0) {
			std::cout << std::endl;
			std::cout << "Some " << qt_oc << " occurences:" << std::endl << std::endl;
			for (int i = 0; i < qt_oc; i++) {

				int j = std::min(m.match_idx(i), MAX_OC_RANGE);
				if (j < MAX_OC_RANGE) {
					for (int k = 0; k < 3; k++) std::cout << " ";
					for (int k = 0; k < MAX_OC_RANGE-j; k++) std::cout << " ";
				} else std::cout << "...";
				for (int k = 0; k < j; k++) std::cout << t[m.match_idx(i)-j+k];

				std::cout << INVERT(p);

				j = std::min<int>(t.size() - m.match_idx(i) - p.size(), MAX_OC_RANGE);
				for (int k = 0; k < j; k++) std:: cout << t[m.match_idx(i)+p.size()+k];
				if (j < MAX_OC_RANGE) {
					for (int k = 0; k < 3; k++) std::cout << " ";
					for (int k = 0; k < MAX_OC_RANGE-j; k++) std::cout << " ";
				} else std::cout << "...";

				std::cout << "\t(index " << format_int(m.match_idx(i)) << ")";
				std::cout << std::endl;
			}
		}

		terminal::print_cursor();
		char c = terminal::getch(1);
		if (c == 0) {
			continue;
		} else if (c == 3) { // Ctrl+C
			handle_exit_signal();
		} else if (c == 26) { // Ctrl+Z
			continue;
		} else if (c == 27) { // special command
			c = terminal::getch_now();
			if (c == 0) { // escape
				break;
			}
			c = terminal::getch(1);
			if (c == 'D') { // left arrow
				cursor_pos = std::max<int>(0, cursor_pos - 1);
			} else if (c == 'C') { // right arrow
				cursor_pos = std::min<int>(p.size(), cursor_pos + 1);
			} else if (c == 'B') { // down arrow
				int qt_oc = std::min(MAX_OC_PRINT, m.matches());
				occ_shift = std::min(occ_shift + 1, m.matches() - qt_oc);
			} else if (c == 'A') { // up arrow
				occ_shift = std::max(0, occ_shift - 1);
			}
		} else if (c == 127) { // backspace
			if (cursor_pos > 0) {
				cursor_pos--;
				p.erase(p.begin() + cursor_pos);
				timer UPD;
				m.erase(cursor_pos);
				update_time_ms = *UPD;
				occ_shift = 0;
			}
		} else if (valid_char(c)) { // letter
			p.insert(p.begin() + cursor_pos, c);
			timer UPD;
			m.insert(cursor_pos, c);
			update_time_ms = *UPD;
			cursor_pos++;
			occ_shift = 0;
		}
	}
}

int main(int argc, char** argv) {
	setup();
	if (argc < 2) {
		std::cout << "Error: please provide an input file" << std::endl;
		return 1;
	}
	std::string text_name = argv[1];
	std::ifstream in(text_name);
	if (!in.is_open()) {
		std::cout << "Error: could not open file" << std::endl;
		return 1;
	}

	terminal::new_screen();
	terminal::clear_screen();
	terminal::hide_cursor();

	std::string t = read_text(in, text_name);

	dyn_pattern::matching m;
	int build_time_ms = preprocess(m, text_name, t);

	run_matching(m, text_name, t, build_time_ms);

	terminal::close_screen();
	return 0;
}

