#pragma once

#include <vector>
#include <string>
#include <tuple>

#include "rmq.cpp"

namespace dyn_pattern {

struct suffix_array {
	std::string s;
	int n;
	std::vector<int> sa, cnt, rnk, lcp;
	rmq<int> RMQ;
	std::vector<int> L_lcp, R_lcp;

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
	int lcp_precalc(int L, int R) {
		if (R - L <= 1) return lcp[L];
		int M = (L+R)/2;
		L_lcp[M] = lcp_precalc(L, M);
		R_lcp[M] = lcp_precalc(M, R);
		return std::min(L_lcp[M], R_lcp[M]);
	}

	suffix_array() {}
	suffix_array(const std::string& s_) : s(s_), n(s.size()), sa(n+3), cnt(n+1), rnk(n),
		lcp(n-1), L_lcp(n), R_lcp(n) {
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
		lcp_precalc(0, n-1);
	}

	int lcp_query(int l, int r) {
		if (r < l) std::swap(l, r);
		if (l == r) return n - sa[l];
		return RMQ.query(l, r-1);
	}

	// returns a pair {first/last idx, lcp}
	std::pair<int, int> get_idx(std::string& p, bool first) {
		auto extend_lcp = [&](int l, int i) {
			while (l < (int) p.size() and sa[i]+l < (int) s.size()
					and p[l] == s[sa[i]+l]) l++;
			return l;
		};
		int l = extend_lcp(0, 0), r = extend_lcp(0, n-1);

		// occurs at idx 0
		if (first and l == (int) p.size()) return {0, p.size()};
		// occurs at idx n-1
		if (!first and r == (int) p.size()) return {n, p.size()};

		int L = 0, R = n-1;
		while (R - L > 1) {
			int M = (L+R)/2;
			int m;
			if (l >= r) m = L_lcp[M] < l ? L_lcp[M] : extend_lcp(l, M);
			else m = R_lcp[M] < r ? R_lcp[M] : extend_lcp(r, M);

			bool eq = m == (int) p.size();
			bool greater = !eq and sa[M]+m < n and p[m] < s[sa[M]+m];

			bool right = first and (greater or eq);
			right |= !first and greater;

			if (right) R = M, r = m;
			else L = M, l = m;
		}
		int pattern_lcp = std::max(l, r);
		// if pattern_lcp < |P|, then P does not occur
		if (pattern_lcp < (int) p.size()) return {n, pattern_lcp};
		return {R, p.size()};
	}
	// O(log(n) + lcp) <= O(log(n) + |p|)
	std::tuple<int, int, int> pattern_search(std::string& p) {
		if (!p.size()) return {0, 0, 0};
		auto [l, pattern_lcp] = get_idx(p, true);
		if (l == n) return {0, 0, pattern_lcp};
		return {l, get_idx(p, false).first, pattern_lcp};
	}
};

} // namespace dyn_pattern

