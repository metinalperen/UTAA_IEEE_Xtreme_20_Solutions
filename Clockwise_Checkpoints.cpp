/**
 * Problem: Clockwise Checkpoints
 * Competition: IEEE Xtreme 20
 * Score: 72.97 / 72.97 (100%)
 */

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    
    int n;
    long long c;
    if (!(cin >> n >> c)) return 0;
    
    vector<long long> p(n);
    for (int i = 0; i < n; i++) {
        cin >> p[i];
    }
    
    sort(p.begin(), p.end());
    
    long long max_len = 0;
    long long best_start = -1;
    
    for (int i = 0; i < n; i++) {
        long long current_len;
        if (i == n - 1) {
            current_len = p[0] + c - p[i];
        } else {
            current_len = p[i + 1] - p[i];
        }
        
        if (current_len > max_len) {
            max_len = current_len;
            best_start = p[i];
        } else if (current_len == max_len) {
            if (best_start == -1 || p[i] < best_start) {
                best_start = p[i];
            }
        }
    }
    
    cout << max_len << " " << best_start << "\n";
    
    return 0;
}