/**
 * Problem: At Most One Cover
 * Competition: IEEE Xtreme 20
 * Score: 66.88 / 66.88 (100%)
 */

#include <iostream>
#include <vector>

using namespace std;

const long long MOD = 998244353;

long long power(long long base, long long exp) {
    long long res = 1;
    base %= MOD;
    while (exp > 0) {
        if (exp % 2 == 1) res = (res * base) % MOD;
        base = (base * base) % MOD;
        exp /= 2;
    }
    return res;
}

long long modInverse(long long n) {
    return power(n, MOD - 2);
}

struct SegTreeProd {
    int size;
    vector<long long> tree, lazy;
    SegTreeProd(int n) {
        size = 1;
        while (size < n) size *= 2;
        tree.assign(2 * size, 1);
        lazy.assign(2 * size, 1);
    }
    void apply(int node, long long val) {
        tree[node] = (tree[node] * val) % MOD;
        lazy[node] = (lazy[node] * val) % MOD;
    }
    void push(int node) {
        if (lazy[node] != 1) {
            apply(2 * node, lazy[node]);
            apply(2 * node + 1, lazy[node]);
            lazy[node] = 1;
        }
    }
    void update(int l, int r, long long val, int node, int nl, int nr) {
        if (l <= nl && nr <= r) {
            apply(node, val);
            return;
        }
        push(node);
        int mid = (nl + nr) / 2;
        if (l <= mid) update(l, r, val, 2 * node, nl, mid);
        if (r > mid) update(l, r, val, 2 * node + 1, mid + 1, nr);
    }
    void update(int l, int r, long long val) {
        update(l, r, val, 1, 0, size - 1);
    }
    long long query(int idx) {
        int node = 1;
        int nl = 0, nr = size - 1;
        while (nl != nr) {
            push(node);
            int mid = (nl + nr) / 2;
            node = 2 * node;
            if (idx <= mid) {
                nr = mid;
            } else {
                nl = mid + 1;
                node++;
            }
        }
        return tree[node];
    }
};

struct SegTreeSum {
    int size;
    vector<long long> tree, lazy;
    SegTreeSum(int n) {
        size = 1;
        while (size < n) size *= 2;
        tree.assign(2 * size, 0);
        lazy.assign(2 * size, 0);
    }
    void apply(int node, long long val) {
        tree[node] = (tree[node] + val) % MOD;
        lazy[node] = (lazy[node] + val) % MOD;
    }
    void push(int node) {
        if (lazy[node] != 0) {
            apply(2 * node, lazy[node]);
            apply(2 * node + 1, lazy[node]);
            lazy[node] = 0;
        }
    }
    void update(int l, int r, long long val, int node, int nl, int nr) {
        if (l <= nl && nr <= r) {
            apply(node, val);
            return;
        }
        push(node);
        int mid = (nl + nr) / 2;
        if (l <= mid) update(l, r, val, 2 * node, nl, mid);
        if (r > mid) update(l, r, val, 2 * node + 1, mid + 1, nr);
    }
    void update(int l, int r, long long val) {
        update(l, r, val, 1, 0, size - 1);
    }
    long long query(int idx) {
        int node = 1;
        int nl = 0, nr = size - 1;
        while (nl != nr) {
            push(node);
            int mid = (nl + nr) / 2;
            node = 2 * node;
            if (idx <= mid) {
                nr = mid;
            } else {
                nl = mid + 1;
                node++;
            }
        }
        return tree[node];
    }
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    
    int n, m;
    if (!(cin >> n >> m)) return 0;
    
    SegTreeProd st_prod(m + 1);
    SegTreeSum st_zeros(m + 1);
    SegTreeSum st_sum(m + 1);
    
    for (int i = 0; i < n; i++) {
        int l, r;
        long long p, q;
        cin >> l >> r >> p >> q;
        if (p == 0) continue;
        if (p == q) {
            st_zeros.update(l, r, 1);
        } else {
            long long P = (p * modInverse(q)) % MOD;
            long long u = (MOD + 1 - P) % MOD;
            long long v = (P * modInverse(u)) % MOD;
            st_prod.update(l, r, u);
            st_sum.update(l, r, v);
        }
    }
    
    for (int i = 1; i <= m; i++) {
        long long z = st_zeros.query(i);
        long long prod = st_prod.query(i);
        long long sum_v = st_sum.query(i);
        long long ans = 0;
        if (z >= 2) {
            ans = 0;
        } else if (z == 1) {
            ans = prod;
        } else {
            ans = (prod * (1 + sum_v)) % MOD;
        }
        cout << ans << (i == m ? "" : " ");
    }
    cout << "\n";
    
    return 0;
}