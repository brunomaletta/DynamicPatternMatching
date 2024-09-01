#pragma once

#include <vector>
#include <string>
#include <deque>
#include <cassert>

namespace dyn_pattern {

template<typename T> struct rmq {
	std::vector<T> v;
	int n; static const int b = 30;
	std::vector<int> mask, t;

	int op(int x, int y) { return v[x] <= v[y] ? x : y; }
	int msb(int x) { return __builtin_clz(1)-__builtin_clz(x); }
	int small(int r, int sz = b) { return r-msb(mask[r]&((1<<sz)-1)); }
	rmq() {}
	rmq(const std::vector<T>& v_) : v(v_), n(v.size()), mask(n), t(n) {
		for (int i = 0, at = 0; i < n; mask[i++] = at |= 1) {
			at = (at<<1)&((1<<b)-1);
			while (at and op(i-msb(at&-at), i) == i) at ^= at&-at;
		}
		for (int i = 0; i < n/b; i++) t[i] = small(b*i+b-1);
		for (int j = 1; (1<<j) <= n/b; j++) for (int i = 0; i+(1<<j) <= n/b; i++)
			t[n/b*j+i] = op(t[n/b*(j-1)+i], t[n/b*(j-1)+i+(1<<(j-1))]);
	}
	int index_query(int l, int r) {
		if (r-l+1 <= b) return small(r, r-l+1);
		int x = l/b+1, y = r/b-1;
		if (x > y) return op(small(l+b-1), small(r));
		int j = msb(y-x+1);
		int ans = op(small(l+b-1), op(t[n/b*j+x], t[n/b*j+y-(1<<j)+1]));
		return op(ans, small(r));
	}
	T query(int l, int r) { return v[index_query(l, r)]; }
};

struct suffix_array {
	std::string s;
	int n;
	std::vector<int> sa, cnt, rnk, lcp;
	rmq<int> RMQ;

	bool cmp(int a1, int b1, int a2, int b2, int a3=0, int b3=0) {
		return a1 != b1 ? a1 < b1 : (a2 != b2 ? a2 < b2 : a3 < b3);
	}
	template<typename T> void radix(int* fr, int* to, T* r, int N, int k) {
		cnt = std::vector<int>(k+1, 0);
		for (int i = 0; i < N; i++) cnt[r[fr[i]]]++;
		for (int i = 1; i <= k; i++) cnt[i] += cnt[i-1];
		for (int i = N-1; i+1; i--) to[--cnt[r[fr[i]]]] = fr[i];
	}
	void rec(std::vector<int>& v, int k) {
		auto &tmp = rnk, &m0 = lcp;
		int N = v.size()-3, sz = (N+2)/3, sz2 = sz+N/3;
		std::vector<int> R(sz2+3);
		for (int i = 1, j = 0; j < sz2; i += i%3) R[j++] = i;

		radix(&R[0], &tmp[0], &v[0]+2, sz2, k);
		radix(&tmp[0], &R[0], &v[0]+1, sz2, k);
		radix(&R[0], &tmp[0], &v[0]+0, sz2, k);

		int dif = 0;
		int l0 = -1, l1 = -1, l2 = -1;
		for (int i = 0; i < sz2; i++) {
			if (v[tmp[i]] != l0 or v[tmp[i]+1] != l1 or v[tmp[i]+2] != l2)
				l0 = v[tmp[i]], l1 = v[tmp[i]+1], l2 = v[tmp[i]+2], dif++;
			if (tmp[i]%3 == 1) R[tmp[i]/3] = dif;
			else R[tmp[i]/3+sz] = dif;
		}

		if (dif < sz2) {
			rec(R, dif);
			for (int i = 0; i < sz2; i++) R[sa[i]] = i+1;
		} else for (int i = 0; i < sz2; i++) sa[R[i]-1] = i;

		for (int i = 0, j = 0; j < sz2; i++) if (sa[i] < sz) tmp[j++] = 3*sa[i];
		radix(&tmp[0], &m0[0], &v[0], sz, k);
		for (int i = 0; i < sz2; i++)
			sa[i] = sa[i] < sz ? 3*sa[i]+1 : 3*(sa[i]-sz)+2;

		int at = sz2+sz-1, p = sz-1, p2 = sz2-1;
		while (p >= 0 and p2 >= 0) {
			if ((sa[p2]%3==1 and cmp(v[m0[p]], v[sa[p2]], R[m0[p]/3],
							R[sa[p2]/3+sz])) or (sa[p2]%3==2 and cmp(v[m0[p]], v[sa[p2]],
								v[m0[p]+1], v[sa[p2]+1], R[m0[p]/3+sz], R[sa[p2]/3+1])))
				sa[at--] = sa[p2--];
			else sa[at--] = m0[p--];
		}
		while (p >= 0) sa[at--] = m0[p--];
		if (N%3==1) for (int i = 0; i < N; i++) sa[i] = sa[i+1];
	}

	suffix_array() {}
	suffix_array(const std::string& s_) : s(s_), n(s.size()), sa(n+3), cnt(n+1), rnk(n), lcp(n-1) {
		std::vector<int> v(n+3);
		for (int i = 0; i < n; i++) v[i] = i;
		radix(&v[0], &rnk[0], &s[0], n, 256);
		int dif = 1;
		for (int i = 0; i < n; i++)
			v[rnk[i]] = dif += (i and s[rnk[i]] != s[rnk[i-1]]);
		if (n >= 2) rec(v, dif);
		sa.resize(n);

		for (int i = 0; i < n; i++) rnk[sa[i]] = i;
		for (int i = 0, k = 0; i < n; i++, k -= !!k) {
			if (rnk[i] == n-1) {
				k = 0;
				continue;
			}
			int j = sa[rnk[i]+1];
			while (i+k < n and j+k < n and s[i+k] == s[j+k]) k++;
			lcp[rnk[i]] = k;
		}
		RMQ = rmq<int>(lcp);
	}
};

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
			if (sa->RMQ.query(m, x-1) < sz) l = m+1;
			else r = m;
		}
		L = l;
		l = x+1, r = sa->n;
		while (l < r) {
			int m = (l+r)/2;
			if (sa->RMQ.query(x, m-1) >= sz) l = m+1;
			else r = m;
		}
		R = r;
	}
	void pop_right(int k) {
		//assert(L < R);
		sz -= k;
		if (sz <= 0) L = 0, R = sa->n, sz = 0;
		else expand(L);
	}
	void pop_left(int k) {
		//assert(L < R);
		sz -= k;
		if (sz <= 0) L = 0, R = sa->n, sz = 0;
		else expand(sa->rnk[sa->sa[L] + k]);
	}
};

struct matching {
	suffix_array sa;
	std::deque<occ> d;

	matching() {}
	matching(const std::string& text) {
		sa = suffix_array(text);
	}

	void push_right(char c) {
		occ at(&sa, c);
		if (d.size()) {
			occ conc = d.back() + at;
			if (conc.matches()) {
				d.back() = conc;
				return;
			}
		}
		d.push_back(at);
	}
	void push_left(char c) {
		occ at(&sa, c);
		if (d.size()) {
			occ conc = d.front() + at;
			if (conc.matches()) {
				d.front() = conc;
				return;
			}
		}
		d.push_front(at);
	}
	void pop_right() {
		d.back().pop_right(1);
		if (d.back().sz == 0) {
			d.pop_back();
			return;
		}
		if (d.size() > 1) {
			occ conc = d.end()[-2] + d.back();
			if (conc.matches()) {
				d.pop_back();
				d.back() = conc;
			}
		}
	}
	void pop_left() {
		d.front().pop_left(1);
		if (d.front().sz == 0) {
			d.pop_front();
			return;
		}
		if (d.size() > 1) {
			occ conc = d[0] + d[1];
			if (conc.matches()) {
				d.pop_front();
				d.front() = conc;
			}
		}
	}
	void merge() {
		for (int i = 0; i+1 < d.size(); i++) {
			auto o = d[i] + d[i+1];
			if (o.matches() > 0) {
				d[i] = o;
				d.erase(d.begin() + i+1);
				i--;
			}
		}
	}
	void split(int i) { // split i|i+1
		for (int j = 0; j < d.size(); j++) {
			if (i < d[j].sz) {
				if (i+1 == d[j].sz) return;
				auto o = d[j];
				d[j].pop_right(d[j].sz-1 - i);
				o.pop_left(i+1);
				d.insert(d.begin() + j+1, o);
				return;
			}
			i -= d[j].sz;
		}
		assert(false);
	}
	void erase(int i) {
		if (i > 0) split(i-1);
		split(i);

		for (int j = 0; j < d.size(); j++) {
			if (i < d[j].sz) {
				assert(d[j].sz == 1);
				d.erase(d.begin() + j);
				merge();
				return;
			}
			i -= d[j].sz;
		}
		assert(false);
	}
	void insert(int i, char c) {
		occ at(&sa, c);
		if (i > 0) split(i-1);
		for (int j = 0; j < d.size(); j++) {
			if (i < d[j].sz) {
				assert(i == 0);
				d.insert(d.begin() + j, at);
				merge();
				return;
			}
			i -= d[j].sz;
		}
		d.push_back(at);
		merge();
	}
	void clear() {
		d.clear();
	}
	int matches() {
		if (d.size() != 1) return 0;
		return d[0].matches();
	}
};

} // namespace dyn_pattern

