#pragma once

#include <string>
#include <vector>

#include "sa.cpp"
#include "treap.cpp"

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
		if (sa->sa[l] + S.sz == sa->n or !(sa->rnk[sa->sa[l] + S.sz] < T.R)) {
			ret.L = ret.R = 0;
			return ret;
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
		sz -= k;
		if (sz <= 0) L = 0, R = sa->n, sz = 0;
		else expand(L);
	}
	void pop_left(int k) {
		sz -= k;
		if (sz <= 0) L = 0, R = sa->n, sz = 0;
		else expand(sa->rnk[sa->sa[L] + k]);
	}
	std::pair<occ, occ> split(int k) const {
		occ l = *this, r = *this;
		l.pop_right(sz - k);
		r.pop_left(k);
		return {l, r};
	}
};

struct matching {
	suffix_array sa;
	treap<occ> part;

	matching() {}
	matching(const std::string& text) {
		sa = suffix_array(text);
	}

	void erase(int i) {
		treap<occ> L, M;
		part.split(i, L);
		part.split(1, M);
		part.join_left(L);
	}
	void insert(int i, char c) {
		treap<occ> L;
		part.split(i, L);
		treap<occ> M(occ(&sa, c));
		part.join_left(M);
		part.join_left(L);
	}
	void insert(int i, const std::string& s) {
		treap<occ> L;
		part.split(i, L);
		for (int i = 0; i < s.size(); i++) {
			treap<occ> M(occ(&sa, s.rbegin()[i]));
			part.join_left(M);
		}
		part.join_left(L);
	}
	void clear() {
		treap<occ> tmp;
		swap(tmp, part);
	}
	int matches() {
		if (!part.one_node()) return 0;
		return part.root->val.matches();
	}
	int match_idx(int i) {
		return sa.sa[part.root->val.L+i];
	}
	//std::pair<int, int> range() { return {part.root->val.L, part.root->val.R}; }
};

} // namespace dyn_pattern

