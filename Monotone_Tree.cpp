/**
 * Problem: Monotone Tree
 * Competition: IEEE Xtreme 20
 * Score: 69.38 / 69.38 (100%)
 */

#include <bits/stdc++.h>
using namespace std;

using ll = long long;
using i128 = __int128_t;

struct Fraction {
    ll num, den;
    
    Fraction(ll n = 0, ll d = 1) {
        if (d < 0) {
            n = -n;
            d = -d;
        }
        ll g = gcd(llabs(n), d);
        num = n / g;
        den = d / g;
    }
};

struct Compare {
    bool operator()(const Fraction& a,
    const Fraction& b) const {
        return (i128)a.num * b.den <
        (i128)b.num * a.den;
    }
};

using Heap = map<Fraction, ll, Compare>;

bool lessEqual(const Fraction& a, ll num, ll den) {
    return (i128)a.num * den <=
    (i128)num * a.den;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    int n;
    cin >> n;
    
    vector<int> p(n + 1);
    vector<ll> a(n + 1), w(n + 1);
    
    vector<vector<int>> children(n + 1);
    
    for (int i = 1; i <= n; i++) {
        cin >> p[i] >> a[i] >> w[i];
        
        if (p[i] != 0) {
            children[p[i]].push_back(i);
        }
    }
    
    // Root-first traversal
    vector<int> order;
    order.reserve(n);
    order.push_back(1);
    
    for (int i = 0; i < (int)order.size(); i++) {
        int u = order[i];
        
        for (int v : children[u]) {
            order.push_back(v);
        }
    }
    
    vector<Heap*> heaps(n + 1, nullptr);
    vector<Fraction> opt(n + 1);
    
    // Bottom-up DP
    for (int i = n - 1; i >= 0; i--) {
        int u = order[i];
        
        // Find the largest child heap
        int big = -1;
        
        for (int v : children[u]) {
            if (big == -1 ||
            heaps[v]->size() > heaps[big]->size()) {
                big = v;
            }
        }
        
        Heap* cur;
        
        if (big == -1) {
            cur = new Heap();
        } else {
            cur = heaps[big];
            heaps[big] = nullptr;
        }
        
        // Merge smaller heaps into the largest heapsfor (int v : children[u]) {
        
        for(int v : children[u]){
            if (v == big) continue;
        
            for (const auto& entry : *heaps[v]) {
                auto it = cur->find(entry.first);
            
                if (it == cur->end()) {
                    cur->insert(entry);
                } else {
                    it->second += entry.second;
                }   
            }
        
            delete heaps[v];
            heaps[v] = nullptr;
        }
    
    
    // Calculate the minimum of the convex function
    ll num = a[u] * w[u];
    ll den = w[u];
    
    while (!cur->empty()) {
        auto it = cur->begin();
        
        Fraction t = it->first;
        ll c = it->second;
        
        if (!lessEqual(t, num, den)) {
            break;
        }
        
        // c * t is guaranteed to be an integer
        i128 contribution =
        (i128)c * t.num / t.den;
        
        num += (ll)contribution;
        den += c;
        
        cur->erase(it);
    }
    
    // Store the optimal value
    opt[u] = Fraction(num, den);
    
    // Positive part representation:
    // den * max(0, x - opt[u])
    (*cur)[opt[u]] += den;
    
    heaps[u] = cur;
}

// Top-down reconstruction
vector<Fraction> ans(n + 1);

for (int u : order) {
    if (u == 1) {
        ans[u] = opt[u];
    } else {
        int par = p[u];
        
        if ((i128)opt[u].num * ans[par].den >=
        (i128)ans[par].num * opt[u].den) {
            ans[u] = opt[u];
        } else {
            ans[u] = ans[par];
        }
    }
}

// Output
for (int u = 1; u <= n; u++) {
    cout << ans[u].num << " "
    << ans[u].den << '\n';
}

// Release memory
for (int u = 1; u <= n; u++) {
    delete heaps[u];
}

return 0;
}
