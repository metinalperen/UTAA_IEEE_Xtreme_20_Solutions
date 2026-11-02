/**
 * Problem: One Junction Box
 * Competition: IEEE Xtreme 20
 * Score: 55.79 / 69.74 (80.00%)
 */

#pragma GCC optimize("O3,unroll-loops")
#pragma GCC target("avx2,bmi,bmi2,lzcnt,popcnt")
#include <iostream>
#include <vector>

using namespace std;

const int MOD = 998244353;
const int MAX = 3000005;

long long fact[MAX];
long long invFact[MAX];

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

void precompute() {
    fact[0] = 1;
    invFact[0] = 1;
    for (int i = 1; i < MAX; i++) {
        fact[i] = (fact[i - 1] * i) % MOD;
    }
    invFact[MAX - 1] = modInverse(fact[MAX - 1]);
    for (int i = MAX - 2; i >= 1; i--) {
        invFact[i] = (invFact[i + 1] * (i + 1)) % MOD;
    }
}

long long nCr(int n, int r) {
    if (r < 0 || r > n) return 0;
    long long num = fact[n];
    long long den = (invFact[r] * invFact[n - r]) % MOD;
    return (num * den) % MOD;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    
    precompute();
    
    int n, m;
    if (!(cin >> n >> m)) return 0;
    
    long long total_layouts = 0;
    
    for (int x = 1; x <= n - 2; x++) {
        long long current_row_sum = 0;
        
        for (int y = 1; y <= m - 2; y++) {
            long long w1 = nCr(x + y - 1, x - 1);
            long long w4 = nCr(x + m - y - 2, x);
            long long w3 = nCr(n - x + y - 2, y - 1);
            long long w2 = nCr(n - x + m - y - 3, m - y - 1);
            
            long long ways = (w1 * w4) % MOD;
            ways = (ways * w3) % MOD;
            ways = (ways * w2) % MOD;
            
            current_row_sum = (current_row_sum + ways) % MOD;
        }
        total_layouts = (total_layouts + current_row_sum) % MOD;
    }
    
    // İki farklı kesişme senaryosu olduğu için 2 ile çarpıyoruz
    total_layouts = (total_layouts * 2) % MOD;
    
    cout << total_layouts << "\n";
    
    return 0;
}