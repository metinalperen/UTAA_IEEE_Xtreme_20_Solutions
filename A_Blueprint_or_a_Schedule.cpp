/**
 * Problem: A Blueprint or a Schedule
 * Competition: IEEE Xtreme 20
 * Score: 69.88 / 74.25 (94.12%)
 */

#include <iostream>
#include <vector>
#include <queue>
#include <cstring>
#include <algorithm>

using namespace std;

const int N = 20005;
const int K = 205;

vector<int> adj[N], radj[N];
vector<pair<int, int>> bp[K];
vector<int> ch[K], edir[K];

int L[N], deg[N];
bool dp_val[N][K];
bool vis_tree[K];
bool vis[N];
int ans[K];
bool can[N];
int n, m, k;

void build(int u) {
    vis_tree[u] = true;
    for (auto p : bp[u]) {
        int v = p.first;
        int d = p.second;
        if (!vis_tree[v]) {
            ch[u].push_back(v);
            edir[u].push_back(d);
            build(v);
        }
    }
}

void calc_dp(int i) {
    for (int j : ch[i]) calc_dp(j);
    
    for (int idx = 0; idx < (int)ch[i].size(); ++idx) {
        int j = ch[i][idx];
        int d = edir[i][idx];
        
        memset(can, 0, sizeof(can));
        
        if (d == 1) {
            for (int v = 1; v <= n; ++v) {
                if (dp_val[v][j]) {
                    for (int u : radj[v]) can[u] = true;
                }
            }
        } else {
            for (int v = 1; v <= n; ++v) {
                if (dp_val[v][j]) {
                    for (int u : adj[v]) can[u] = true;
                }
            }
        }
        
        for (int u = 1; u <= n; ++u) {
            dp_val[u][i] = dp_val[u][i] && can[u];
        }
    }
}

void undo(int i) {
    vis[ans[i]] = false;
    for (int j : ch[i]) undo(j);
}

bool dfs2(int i, int cidx, int u);

bool dfs1(int i, int u) {
    ans[i] = u;
    vis[u] = true;
    if (dfs2(i, 0, u)) return true;
    vis[u] = false;
    return false;
}

bool dfs2(int i, int cidx, int u) {
    if (cidx == (int)ch[i].size()) return true;
    int j = ch[i][cidx];
    int d = edir[i][cidx];
    
    if (d == 1) {
        for (int v : adj[u]) {
            if (!vis[v] && dp_val[v][j]) {
                if (dfs1(j, v)) {
                    if (dfs2(i, cidx + 1, u)) return true;
                    undo(j);
                }
            }
        }
    } else {
        for (int v : radj[u]) {
            if (!vis[v] && dp_val[v][j]) {
                if (dfs1(j, v)) {
                    if (dfs2(i, cidx + 1, u)) return true;
                    undo(j);
                }
            }
        }
    }
    return false;
}

int main() {
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    
    if (!(cin >> n >> m >> k)) return 0;
    
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        adj[u].push_back(v);
        radj[v].push_back(u);
    }
    
    for (int i = 0; i < k - 1; ++i) {
        int u, v;
        cin >> u >> v;
        bp[u].push_back({v, 1});
        bp[v].push_back({u, -1});
    }
    
    for (int i = 1; i <= n; ++i) {
        deg[i] = adj[i].size();
        L[i] = 1;
    }
    
    queue<int> q;
    for (int i = 1; i <= n; ++i) {
        if (deg[i] == 0) q.push(i);
    }
    
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (int v : radj[u]) {
            L[v] = max(L[v], L[u] + 1);
            deg[v]--;
            if (deg[v] == 0) q.push(v);
        }
    }
    
    int mx = 0;
    for (int i = 1; i <= n; ++i) mx = max(mx, L[i]);
    
    if (mx < k) {
        cout << "COLOR\n";
        for (int i = 1; i <= n; ++i) {
            cout << L[i] << (i == n ? "" : " ");
        }
        cout << "\n";
        return 0;
    }
    
    memset(dp_val, 1, sizeof(dp_val));
    build(1);
    calc_dp(1);
    
    for (int i = 1; i <= n; ++i) {
        if (dp_val[i][1]) {
            if (dfs1(1, i)) {
                cout << "TREE\n";
                for (int j = 1; j <= k; ++j) {
                    cout << ans[j] << (j == k ? "" : " ");
                }
                cout << "\n";
                return 0;
            }
        }
    }
    
    return 0;
}