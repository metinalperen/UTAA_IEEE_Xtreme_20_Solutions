/**
 * Problem: Lunar Giant
 * Competition: IEEE Xtreme 20
 * Score: 80.07 / 80.07 (100%)
 */

#include <iostream>
#include <vector>
#include <string>
#include <sstream>

using namespace std;

const int BASE[210] = {
    0, 148, 117, 45, 192, 68, 166, 101, 208, 60, 78, 152, 120, 195, 196, 5, 71, 123, 136, 15, 82,
    165, 22, 8, 112, 93, 185, 85, 126, 23, 31, 49, 37, 177, 158, 118, 98, 156, 140, 62, 38, 128,
    184, 46, 35, 50, 104, 74, 36, 167, 55, 79, 77, 72, 54, 153, 129, 143, 162, 199, 9, 125, 87,
    64, 142, 175, 11, 146, 181, 58, 70, 6, 100, 191, 40, 182, 110, 119, 150, 102, 3, 113, 20, 19,
    180, 134, 4, 39, 197, 88, 161, 2, 188, 30, 65, 202, 41, 26, 52, 133, 33, 114, 94, 103, 193,
    16, 106, 115, 95, 176, 76, 157, 183, 168, 7, 144, 179, 21, 207, 48, 121, 12, 170, 205, 75,
    29, 190, 189, 96, 206, 107, 59, 90, 99, 27, 169, 18, 109, 203, 139, 151, 28, 63, 198, 34, 67,
    145, 122, 84, 200, 10, 47, 66, 80, 56, 155, 137, 132, 130, 154, 42, 173, 135, 105, 159, 174,
    163, 25, 81, 171, 147, 69, 53, 111, 91, 51, 32, 172, 160, 178, 186, 83, 124, 24, 116, 97,
    201, 187, 44, 127, 194, 73, 86, 138, 204, 13, 14, 89, 57, 131, 149, 1, 108, 43, 141, 17, 164,
    92, 61, 209
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    
    string line;
    if (!getline(cin, line)) return 0;
    while (line.empty() && getline(cin, line));
    int N = stoi(line);
    
    for (int t = 0; t < N; ++t) {
        if (!getline(cin, line)) break;
        while (line.empty() && getline(cin, line));
        stringstream ss(line);
        vector<int> c;
        int x;
        while (ss >> x) c.push_back(x);
        
        int c1 = c[0];
        int tape_after[3] = {0, 0, 0};
        int idx = 0;
        for (size_t i = 1; i < c.size() && idx < 3; ++i) {
            for (int j = 0; j < c[i] && idx < 3; ++j) {
                tape_after[idx++] = 1;
            }
            if (idx < 3) {
                tape_after[idx++] = 0;
            }
        }
        
        int b2 = tape_after[1];
        int b3 = tape_after[2];
        int eq = (c1 + 126 * (2 * b2 + b3)) % 210;
        cout << BASE[eq] << "\n";
    }
    
    return 0;
}