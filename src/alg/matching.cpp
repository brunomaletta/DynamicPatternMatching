#pragma once

#include <string>
#include <vector>
#include <cassert>

#include "sa.cpp"

namespace dyn_pattern {

struct occ {
	int L, R, sz;
	suffix_array* sa;

	occ(suffix_array* sa_) : L(0), R(sa_->n), sz(0), sa(sa_) {}
	occ(suffix_array* sa_, char c) {
		sa = sa_;
		sz = 1;
		int l = 0, r = sa->n;
		while (l < r) {
			int m = (l+r)/2;
			if (sa->s[sa->sa[m]] < c) l = m+1;
			else r = m;
		}
		if (l == sa->n or sa->s[sa->sa[l]] > c) {
			L = R = 0;
			return;
		}
		L = l;
		r = sa->n;
		while (l < r) {
			int m = (l+r)/2;
			if (sa->s[sa->sa[m]] <= c) l = m+1;
			else r = m;
		}
		R = r;
	}

	int matches() { return R-L; }

	friend occ operator +(occ S, occ T) {
		suffix_array* sa = S.sa;
		occ ret(sa);
		ret.sz = S.sz + T.sz;
		int l = S.L, r = S.R;
		while (l < r) {
			int m = (l+r)/2;
			if (sa->sa[m] + S.sz == sa->n or sa->rnk[sa->sa[m] + S.sz] < T.L)
				l = m+1;
			else r = m;
		}
		ret.L = l;
		r = S.R;
		while (l < r) {
			int m = (l+r)/2;
			if (sa->sa[m] + S.sz == sa->n or sa->rnk[sa->sa[m] + S.sz] < T.R)
				l = m+1;
			else r = m;
		}
		ret.R = r;
		return ret;
	}

	void expand(int x) {
		int l = 0, r = x;
		while (l < r) {
			int m = (l+r)/2;
			if (sa->lcp_query(m, x) < sz) l = m+1;
			else r = m;
		}
		L = l;
		l = x+1, r = sa->n;
		while (l < r) {
			int m = (l+r)/2;
			if (sa->lcp_query(x, m) >= sz) l = m+1;
			else r = m;
		}
		R = r;
	}
	void pop_right(int k) {
		assert(L < R);
		sz -= k;
		if (sz <= 0) L = 0, R = sa->n, sz = 0;
		else expand(L);
	}
	void pop_left(int k) {
		assert(L < R);
		sz -= k;
		if (sz <= 0) L = 0, R = sa->n, sz = 0;
		else expand(sa->rnk[sa->sa[L] + k]);
	}
};

struct matching {
	suffix_array sa;
	std::vector<occ> part;

	matching() {}
	matching(const std::string& text) {
		sa = suffix_array(text);
	}

	void merge() {
		for (int i = 0; i+1 < (int) part.size(); i++) {
			auto o = part[i] + part[i+1];
			if (o.matches() > 0) {
				part[i] = o;
				part.erase(part.begin() + i+1);
				i--;
			}
		}
	}
	void split(int i) { // split i|i+1
		for (int j = 0; j < (int) part.size(); j++) {
			if (i < part[j].sz) {
				if (i+1 == part[j].sz) return;
				auto o = part[j];
				part[j].pop_right(part[j].sz-1 - i);
				o.pop_left(i+1);
				part.insert(part.begin() + j+1, o);
				return;
			}
			i -= part[j].sz;
		}
		assert(false);
	}
	void erase(int i) {
		if (i > 0) split(i-1);
		split(i);

		for (int j = 0; j < (int) part.size(); j++) {
			if (i < part[j].sz) {
				assert(part[j].sz == 1);
				part.erase(part.begin() + j);
				merge();
				return;
			}
			i -= part[j].sz;
		}
		assert(false);
	}
	void insert(int i, char c) {
		occ at(&sa, c);
		if (i > 0) split(i-1);
		for (int j = 0; j < (int) part.size(); j++) {
			if (i < part[j].sz) {
				assert(i == 0);
				part.insert(part.begin() + j, at);
				merge();
				return;
			}
			i -= part[j].sz;
		}
		part.push_back(at);
		merge();
	}
	void clear() {
		part.clear();
	}
	int matches() {
		if (part.size() != 1) return 0;
		return part[0].matches();
	}
	std::pair<int, int> range() {
		return {part[0].L, part[0].R};
	}
};

} // namespace dyn_pattern

