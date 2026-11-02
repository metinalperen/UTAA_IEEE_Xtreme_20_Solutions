/**
 * Problem: Overclock in Night City
 * Competition: IEEE Xtreme 20
 * Score: 55.08 / 67.80 (81.25%)
 */

#include <bits/stdc++.h>
using namespace std;

using ll = long long;
using i128 = __int128_t;

bool check(ll X, const vector<ll>& a) {
    priority_queue<ll> pq;
    
    for (ll x : a) {
        pq.push(x);
    }
    
    ll k = 0;
    
    while (!pq.empty()) {
        ll limit = X + k;
        ll largest = pq.top();
        
        // Smasher can force an infinite simulation.
        if ((i128)largest > 2 * (i128)limit + 1) {
            return false;
        }
        
        // David removes soldiers from the largest squad.
        pq.pop();
        
        ll remaining = largest - limit;
        
        if (remaining > 0) {
            pq.push(remaining);
        }
        
        // All squads are empty.
        if (pq.empty()) {
            return true;
        }
        
        // Smasher doubles the largest remaining squad.
        ll smasher = pq.top();
        pq.pop();
        
        ll nextLimit = X + k + 1;
        
        // Check whether doubling creates an infinite strategy.
        if (2 * (i128)smasher > 2 * (i128)nextLimit + 1) {
            return false;
        }
        
        pq.push(smasher * 2);
        
        k++;
    }
    
    return true;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    int N;
    cin >> N;
    
    vector<ll> a(N);
    
    ll mx = 0;
    
    for (int i = 0; i < N; i++) {
        cin >> a[i];
        mx = max(mx, a[i]);
    }
    
    ll low = 1;
    ll high = mx;
    
    while (low < high) {
        ll mid = low + (high - low) / 2;
        
        if (check(mid, a)) {
            high = mid;
        } else {
            low = mid + 1;
        }
    }
    
    cout << low << '\n';
    
    return 0;
}