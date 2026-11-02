/**
 * Problem: Deliveries in the Kingdom
 * Competition: IEEE Xtreme 20
 * Score: 75.92 / 75.92 (100%)
 */

#include <bits/stdc++.h>
using namespace std;

#define ll long long
const int LOG = 20;

int n, m, q, timerDFS = 0;

vector<vector<pair<int, int>>> adj;

vector<int> tin, low, tout;
vector<int> parentNode, parentEdge;
vector<int> depth, comp, childCount;
vector<int> art;

vector<vector<int>> up;

void dfs() {
    int cid = 0;
    
    vector<int> idx(n + 1, 0);
    
    for (int s = 1; s <= n; s++) {
        if (tin[s]) continue;
        
        cid++;
        
        vector<int> st;
        
        tin[s] = low[s] = ++timerDFS;
        comp[s] = cid;
        
        st.push_back(s);
        
        while (!st.empty()) {
            int v = st.back();
            
            if (idx[v] < (int)adj[v].size()) {
                auto [to, id] = adj[v][idx[v]++];
                
                if (id == parentEdge[v])
                continue;
                
                if (!tin[to]) {
                    parentNode[to] = v;
                    parentEdge[to] = id;
                    depth[to] = depth[v] + 1;
                    comp[to] = cid;
                    
                    childCount[v]++;
                    
                    tin[to] = low[to] = ++timerDFS;
                    
                    up[to][0] = v;
                    
                    for (int j = 1; j < LOG; j++) {
                        up[to][j] =
                        up[up[to][j - 1]][j - 1];
                    }
                    
                    st.push_back(to);
                }
                else {
                    low[v] = min(low[v], tin[to]);
                }
            }
            else {
                st.pop_back();
                
                tout[v] = timerDFS;
                
                int p = parentNode[v];
                
                if (p != 0) {
                    low[p] = min(low[p], low[v]);
                    
                    if (parentNode[p] != 0 &&
                    low[v] >= tin[p]) {
                        art[p] = 1;
                    }
                }
                else {
                    if (childCount[v] > 1)
                    art[v] = 1;
                }
            }
        }
    }
}

int lift(int v, int k) {
    for (int j = 0; j < LOG; j++) {
        if (k & (1 << j))
        v = up[v][j];
    }
    
    return v;
}

bool isAncestor(int u, int v) {
    return tin[u] <= tin[v] &&
    tout[v] <= tout[u];
}

// E'nin alt ağacında x'in bulunduğu
// ilk DFS çocuğunu bulur.
int getChild(int e, int x) {
    int diff = depth[x] - depth[e] - 1;
    return lift(x, diff);
}

// E silindiğinde x'in hangi bileşende
// kaldığını belirler.
int getGroup(int e, int x) {
    
    // E, x'in atası değilse
    // parent tarafındaki bileşende.
    if (!isAncestor(e, x))
    return 0;
    
    int child = getChild(e, x);
    
    // DFS kökü silinirse her çocuk
    // ayrı bir bileşen oluşturur.
    if (parentNode[e] == 0)
    return child;
    
    // Bu alt ağaç E'den yukarıya
    // geri ulaşamıyorsa ayrı bileşendir.
    if (low[child] >= tin[e])
    return child;
    
    // Diğer bütün düğümler aynı bileşende.
    return 0;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    cin >> n >> m;
    
    adj.resize(n + 1);
    
    for (int i = 0; i < m; i++) {
        int a, b;
        cin >> a >> b;
        
        adj[a].push_back({b, i});
        adj[b].push_back({a, i});
    }
    
    tin.assign(n + 1, 0);
    low.assign(n + 1, 0);
    tout.assign(n + 1, 0);
    
    parentNode.assign(n + 1, 0);
    parentEdge.assign(n + 1, -1);
    
    depth.assign(n + 1, 0);
    comp.assign(n + 1, 0);
    childCount.assign(n + 1, 0);
    
    art.assign(n + 1, 0);
    
    up.assign(n + 1, vector<int>(LOG, 0));
    
    dfs();
    
    cin >> q;
    
    while (q--) {
        int c, d, e;
        cin >> c >> d >> e;
        
        // Enderman başlangıç veya varış şehrindeyse
        if (c == e || d == e) {
            cout << "Nao\n";
            continue;
        }
        
        // Başlangıç ve varış zaten bağlı değilse
        if (comp[c] != comp[d]) {
            cout << "Nao\n";
            continue;
        }
        
        // E farklı bir connected component içindeyse
        if (comp[c] != comp[e]) {
            cout << "Sim\n";
            continue;
        }
        
        // E articulation point değilse
        if (!art[e]) {
            cout << "Sim\n";
            continue;
        }    
            // aynı bileşende kalıp kalmadığını kontrol et.
            int gc = getGroup(e, c);
            int gd = getGroup(e, d);
            
            cout << (gc == gd ? "Sim\n" : "Nao\n");
        }
        
        return 0;
    }
    