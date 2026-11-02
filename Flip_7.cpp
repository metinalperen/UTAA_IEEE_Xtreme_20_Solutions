/**
 * Problem: Flip 7
 * Competition: IEEE Xtreme 20
 * Score: 74.35 / 74.35 (100%)
 */

#include <iostream>
#include <vector>

using namespace std;

int main() {
    int n, p;
    if (!(cin >> n >> p)) return 0;
    
    vector<int> deck(13);
    for (int i = 1; i <= 12; i++) {
        deck[i] = i;
    }
    int deck_size = 78;
    
    vector<int> disc(13, 0);
    vector<int> tab(13, 0);
    vector<bool> act(n, true);
    vector<vector<bool>> has_card(n, vector<bool>(13, false));
    int my_score = 0;
    
    int turn;
    while (cin >> turn) {
        if (turn == -1) break;
        
        if (turn == p) {
            long long exp_num = 0;
            for (int v = 1; v <= 12; v++) {
                if (deck[v] > 0 && !has_card[p][v]) {
                    exp_num += (long long)deck[v] * (my_score + v);
                }
            }
            
            if (exp_num > (long long)deck_size * my_score) {
                cout << 1 << endl;
                int c;
                cin >> c;
                deck[c]--;
                tab[c]++;
                deck_size--;
                
                if (deck_size == 0) {
                    for (int v = 1; v <= 12; v++) {
                        deck[v] = disc[v];
                        disc[v] = 0;
                        deck_size += deck[v];
                    }
                }
                
                if (has_card[p][c]) {
                    act[p] = false;
                    my_score = 0;
                } else {
                    has_card[p][c] = true;
                    my_score += c;
                }
            } else {
                cout << 0 << endl;
                act[p] = false;
            }
        } else {
            int action;
            cin >> action;
            if (action == 0) {
                act[turn] = false;
            } else {
                int c;
                cin >> c;
                deck[c]--;
                tab[c]++;
                deck_size--;
                
                if (deck_size == 0) {
                    for (int v = 1; v <= 12; v++) {
                        deck[v] = disc[v];
                        disc[v] = 0;
                        deck_size += deck[v];
                    }
                }
                
                if (has_card[turn][c]) {
                    act[turn] = false;
                } else {
                    has_card[turn][c] = true;
                }
            }
        }
        
        bool any_act = false;
        for (int i = 0; i < n; i++) {
            if (act[i]) {
                any_act = true;
                break;
            }
        }
        
        if (!any_act) {
            for (int v = 1; v <= 12; v++) {
                disc[v] += tab[v];
                tab[v] = 0;
            }
            for (int i = 0; i < n; i++) {
                act[i] = true;
                for (int v = 1; v <= 12; v++) {
                    has_card[i][v] = false;
                }
            }
            my_score = 0;
        }
    }
    return 0;
}