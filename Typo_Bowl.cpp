/**
 * Problem: Typo Bowl
 * Competition: IEEE Xtreme 20
 * Score: 76.75 / 76.75 (100%)
 */

#include <bits/stdc++.h>
using namespace std;

int editDistance(const string& a, const string& b) {
    int n = a.size();
    int m = b.size();
    
    vector<int> prev(m + 1), curr(m + 1);
    
    for (int j = 0; j <= m; j++)
    prev[j] = j;
    
    for (int i = 1; i <= n; i++) {
        curr[0] = i;
        
        for (int j = 1; j <= m; j++) {
            int cost = (a[i - 1] == b[j - 1]) ? 0 : 1;
            
            curr[j] = min({
                prev[j] + 1,
                curr[j - 1] + 1,
                prev[j - 1] + cost
            });
        }
        
        swap(prev, curr);
    }
    
    return prev[m];
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    int N;
    cin >> N;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    string openingBand;
    getline(cin, openingBand);
    
    vector<string> afc(N);
    
    for (int i = 0; i < N; i++)
    getline(cin, afc[i]);
    
    string halftimeBand;
    getline(cin, halftimeBand);
    
    vector<string> nfc(N);
    
    for (int i = 0; i < N; i++)
    getline(cin, nfc[i]);
    
    string bestAFC, bestNFC;
    int minAFC = INT_MAX;
    int minNFC = INT_MAX;
    
    for (const string& team : afc) {
        int d = editDistance(openingBand, team);
        
        if (d < minAFC) {
            minAFC = d;
            bestAFC = team;
        }
    }
    
    for (const string& team : nfc) {
        int d = editDistance(halftimeBand, team);
        
        if (d < minNFC) {
            minNFC = d;
            bestNFC = team;
        }
    }
    
    cout << bestAFC << " x " << bestNFC << '\n';
    
    return 0;
}
