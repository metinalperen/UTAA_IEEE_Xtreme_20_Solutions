/**
 * Problem: Rooted Vector
 * Competition: IEEE Xtreme 20
 * Score: 72.50 / 72.50 (100%)
 */

#include <iostream>
#include <vector>

using namespace std;

typedef __int128_t int128;

struct Point {
    long long x, y;
};

int n;
vector<vector<int>> adj;
vector<long long> A, B;
vector<int> q;
vector<int128> dp_val;
vector<long long> dp_A, dp_B;

int128 max_ans = 0;

void update_ans(Point p) {
    int128 cur = (int128)p.x * p.x + (int128)p.y * p.y;
    if (cur > max_ans) max_ans = cur;
}

Point get_best(long long dx, long long dy) {
    for (int i = n - 1; i >= 0; --i) {
        int u = q[i];
        dp_A[u] = A[u];
        dp_B[u] = B[u];
        dp_val[u] = (int128)dx * A[u] + (int128)dy * B[u];
        
        for (int v : adj[u]) {
            if (dp_val[v] > 0) {
                dp_val[u] += dp_val[v];
                dp_A[u] += dp_A[v];
                dp_B[u] += dp_B[v];
            }
        }
    }
    return {dp_A[1], dp_B[1]};
}

void solve(Point P, Point Q) {
    long long dx = P.y - Q.y;
    long long dy = Q.x - P.x;
    
    Point R = get_best(dx, dy);
    update_ans(R);
    
    int128 dot_R = (int128)dx * R.x + (int128)dy * R.y;
    int128 dot_P = (int128)dx * P.x + (int128)dy * P.y;
    
    if (dot_R > dot_P) {
        solve(P, R);
        solve(R, Q);
    }
}

void print128(int128 n) {
    if (n == 0) {
        cout << 0 << "\n";
        return;
    }
    int128 temp = n;
    char buffer[50];
    int idx = 0;
    while (temp > 0) {
        buffer[idx++] = '0' + (temp % 10);
        temp /= 10;
    }
    for (int i = idx - 1; i >= 0; --i) {
        cout << buffer[i];
    }
    cout << "\n";
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    
    if (!(cin >> n)) return 0;
    
    adj.resize(n + 1);
    A.resize(n + 1);
    B.resize(n + 1);
    dp_val.resize(n + 1);
    dp_A.resize(n + 1);
    dp_B.resize(n + 1);
    
    for (int i = 1; i <= n; ++i) {
        int p;
        cin >> p >> A[i] >> B[i];
        if (p != 0) {
            adj[p].push_back(i);
        }
    }
    
    q.push_back(1);
    int head = 0;
    while (head < (int)q.size()) {
        int u = q[head++];
        for (int v : adj[u]) {
            q.push_back(v);
        }
    }
    
    Point P1 = get_best(1, 0);
    Point P2 = get_best(-1, 0);
    
    update_ans(P1);
    update_ans(P2);
    
    solve(P1, P2);
    solve(P2, P1);
    
    print128(max_ans);
    
    return 0;
}