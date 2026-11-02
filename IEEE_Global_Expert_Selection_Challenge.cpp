/**
 * Problem: IEEE Global Expert Selection Challenge
 * Competition: IEEE Xtreme 20
 * Score: 37.87 / 75.74 (50.00%)
 */

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <sstream>
#include <cmath>

using namespace std;

string trim(string s) {
    size_t start = s.find_first_not_of(" \t\r\n\"");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n\"");
    return s.substr(start, end - start + 1);
}

string to_lower(string s) {
    for (char &c : s) c = tolower((unsigned char)c);
    return s;
}

vector<string> parse_csv(string line) {
    vector<string> res;
    string cur = "";
    bool in_q = false;
    for (int i = 0; i < (int)line.size(); i++) {
        char c = line[i];
        if (c == '"') {
            if (in_q && i + 1 < (int)line.size() && line[i+1] == '"') {
                cur += '"';
                i++;
            } else {
                in_q = !in_q;
            }
        } else if (c == ',' && !in_q) {
            res.push_back(trim(cur));
            cur = "";
        } else {
            cur += c;
        }
    }
    res.push_back(trim(cur));
    return res;
}

int get_edu(string e) {
    e = to_lower(e);
    if (e == "phd" || e == "ph.d." || e == "ph.d") return 4;
    if (e == "masters" || e == "maters" || e == "master") return 3;
    if (e == "undergraduate" || e == "undergrad" || e == "bachelors" || e == "bachelor") return 2;
    if (e == "associates" || e == "associate") return 1;
    return 0;
}

int get_tz(string tz) {
    if (tz.empty()) return 0;
    int sign = 1;
    int i = 0;
    if (tz[0] == '-') { sign = -1; i = 1; }
    else if (tz[0] == '+') { sign = 1; i = 1; }
    int h = 0, m = 0;
    size_t pos = tz.find(':', i);
    if (pos != string::npos) {
        try {
            h = stoi(tz.substr(i, pos - i));
            m = stoi(tz.substr(pos + 1));
        } catch(...) {}
    } else {
        try { h = stoi(tz.substr(i)); } catch(...) {}
    }
    return sign * (h * 60 + m);
}

bool has_domain(string d, string req) {
    req = to_lower(req);
    stringstream ss(d);
    string item;
    while (getline(ss, item, ';')) {
        if (to_lower(trim(item)) == req) return true;
    }
    return false;
}

struct Expert {
    string id;
    string orig;
    int edu;
    int exp;
    int tz;
    int tz_diff;
    string domains;
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    
    string line1;
    if (!getline(cin, line1)) return 0;
    if (!line1.empty() && line1.back() == '\r') line1.pop_back();
    
    if (line1.size() >= 3 && (unsigned char)line1[0] == 0xEF && (unsigned char)line1[1] == 0xBB && (unsigned char)line1[2] == 0xBF) {
        line1 = line1.substr(3);
    }
    
    vector<string> crit = parse_csv(line1);
    if (crit.size() < 6) return 0;
    
    string chair_id = crit[0];
    int req_count = 0, req_exp = 0, max_tz = 0;
    try { req_count = stoi(crit[1]); } catch(...) {}
    int req_edu = get_edu(crit[2]);
    try { req_exp = stoi(crit[3]); } catch(...) {}
    string req_domain = crit[4];
    try { max_tz = round(stod(crit[5]) * 60); } catch(...) {}
    
    string line2;
    if (!getline(cin, line2)) return 0;
    if (!line2.empty() && line2.back() == '\r') line2.pop_back();
    
    vector<string> heads = parse_csv(line2);
    int c_id = -1, c_edu = -1, c_exp = -1, c_tz = -1, c_dom = -1;
    for (int i = 0; i < (int)heads.size(); i++) {
        string h = to_lower(trim(heads[i]));
        if (h == "expert id" || h == "expertid") c_id = i;
        else if (h == "education") c_edu = i;
        else if (h == "years of experience" || h == "experience") c_exp = i;
        else if (h == "time zone" || h == "timezone") c_tz = i;
        else if (h == "domains" || h == "domain") c_dom = i;
    }
    
    vector<Expert> exps;
    Expert chair;
    bool found_chair = false;
    
    string line;
    while (getline(cin, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (trim(line).empty()) continue;
        
        vector<string> data = parse_csv(line);
        while (data.size() < heads.size()) data.push_back("");
        
        if (c_id == -1 || c_edu == -1 || c_exp == -1 || c_tz == -1 || c_dom == -1) continue;
        
        Expert e;
        e.orig = line;
        e.id = data[c_id];
        e.edu = get_edu(data[c_edu]);
        try { e.exp = stoi(data[c_exp]); } catch(...) { e.exp = 0; }
        e.tz = get_tz(data[c_tz]);
        e.domains = data[c_dom];
        
        if (e.id == chair_id) {
            chair = e;
            found_chair = true;
        } else {
            exps.push_back(e);
        }
    }
    
    if (!found_chair) return 0;
    
    vector<Expert> valid;
    for (Expert& e : exps) {
        if (e.edu < req_edu) continue;
        if (e.exp < req_exp) continue;
        if (!has_domain(e.domains, req_domain)) continue;
        
        e.tz_diff = abs(e.tz - chair.tz);
        if (e.tz_diff > max_tz) continue;
        
        valid.push_back(e);
    }
    
    sort(valid.begin(), valid.end(), [](const Expert& a, const Expert& b) {
        if (a.edu != b.edu) return a.edu > b.edu;
        if (a.exp != b.exp) return a.exp > b.exp;
        if (a.tz_diff != b.tz_diff) return a.tz_diff < b.tz_diff;
        return a.id < b.id;
    });
    
    int out_count = min((int)valid.size(), req_count);
    vector<Expert> selected(valid.begin(), valid.begin() + out_count);
    
    sort(selected.begin(), selected.end(), [](const Expert& a, const Expert& b) {
        if (a.edu != b.edu) return a.edu > b.edu;
        if (a.exp != b.exp) return a.exp > b.exp;
        return a.id < b.id;
    });
    
    cout << chair.orig << "\n";
    for (int i = 0; i < (int)selected.size(); i++) {
        cout << selected[i].orig << "\n";
    }
    
    return 0;
}