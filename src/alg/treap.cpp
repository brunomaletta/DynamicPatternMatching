#pragma once

#include <random>
#include <chrono>
#include <vector>

namespace dyn_pattern {

template<typename T> struct treap {
	struct node {
		node *l, *r;
		int p, sz;
		T val;
		node(T v, int p_) : l(NULL), r(NULL), p(p_), sz(v.sz), val(v) {}
		void update() {
			sz = val.sz;
			if (l) sz += l->sz;
			if (r) sz += r->sz;
		}
	};

	node* root;
	std::mt19937 rng;

	treap() : root(NULL), rng(std::chrono::steady_clock::now().time_since_epoch().count()) {}
	treap(T v) : treap() { root = new node(v, rng()); }
	~treap() {
		std::vector<node*> q = {root};
		while (q.size()) {
			node* x = q.back(); q.pop_back();
			if (!x) continue;
			q.push_back(x->l), q.push_back(x->r);
			delete x;
		}
	}

	friend void swap(treap& a, treap& b) {
		std::swap(a.root, b.root);
	}
	int size(node* x) { return x ? x->sz : 0; }
	int size() { return size(root); }
	bool one_node() { return root and !root->l and !root->r; }
	void extract_node_left(node* i, node*& l, node*& r) {
		if (!i) return void(l = r = NULL);
		if (i->l) extract_node_left(i->l, l, i->l), r = i;
		else l = i, r = i->r, l->r = NULL;
		i->update();
	}
	void extract_node_right(node* i, node*& l, node*& r) {
		if (!i) return void(l = r = NULL);
		if (i->r) extract_node_right(i->r, i->r, r), l = i;
		else r = i, l = i->l, r->l = NULL;
		i->update();
	}
	void join_values(node*& L, node*& R) {
		if (!L or !R) return;
		T add = L->val + R->val;
		if (add.matches()) {
			L->val = add;
			L->update();
			delete R;
			R = NULL;
		}
	}

	void join(node* l, node* r, node*& i) {
		if (!l or !r) return void(i = l ? l : r);
		if (l->p > r->p) join(l->r, r, l->r), i = l;
		else join(l, r->l, r->l), i = r;
		i->update();
	}
	void join_left(treap& t) {
		node *L, *R;
		extract_node_left(root, R, root);
		extract_node_right(t.root, t.root, L);
		join_values(L, R);
		join(t.root, L, L);
		join(L, R, R);
		join(R, root, root);
		t.root = NULL;
	}

	void fix_left() {
		node *L, *R;
		extract_node_left(root, L, root);
		extract_node_left(root, R, root);
		join_values(L, R);
		join(L, R, R);
		join(R, root, root);
	}
	void fix_right() {
		node *L, *R;
		extract_node_right(root, root, R);
		extract_node_right(root, root, L);
		join_values(L, R);
		join(root, L, root);
		join(root, R, root);
	}

	bool split(node* i, node*& l, node*& r, int idx) {
		if (!i) return (r = l = NULL);
		bool inter_split = false;
		if (size(i->l) + i->val.sz <= idx) {
			inter_split = split(i->r, i->r, r, idx - size(i->l) - i->val.sz);
			l = i;
		} else if (size(i->l) >= idx) {
			inter_split = split(i->l, l, i->l, idx);
			r = i;
		} else {
			idx -= size(i->l);
			auto [L, R] = i->val.split(idx);
			node* n = new node(L, rng());
			join(i->l, n, l);
			i->val = R;
			i->l = NULL;
			r = i;
			inter_split = true;
		}
		i->update();
		return inter_split;
	}
	void split(int idx, treap& t) {
		node *L, *R;
		bool inter_split = split(root, L, R, idx);
		treap tmp;
		tmp.root = L;
		swap(tmp, t);
		root = R;
		if (inter_split) {
			t.fix_right();
			fix_left();
		}
	}
};

} // namespace dyn_pattern

