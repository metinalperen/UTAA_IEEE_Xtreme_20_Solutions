/**
 * Problem: Two Networks, One Emergency
 * Competition: IEEE Xtreme 20
 * Score: 71.54 / 78.94 (90.63%)
 */

#include <bits/stdc++.h>
using namespace std;
using ll = long long;
struct FastInput {
    static const int SIZE = 1 << 20;
    int index = 0, length = 0; char buffer[SIZE];
    char nextChar() {
        if (index == length) length = fread(buffer, 1, SIZE, stdin), index = 0; return length ? buffer[index++] : 0;
    }
    template<class T> void read(T& value) {
        char c;
        do c = nextChar(); while (c <= ' ');
        value = 0;
        do value = value * 10 + c - '0', c = nextChar(); while (c > ' ');
    }
} input;
struct Edge { int to; ll weight; };
struct LCA {
    vector<int> first, depth, logs; vector<ll> rootDistance; vector<vector<int>> table;
    LCA() {}
    LCA(const vector<vector<Edge>>& tree) {
        int n = tree.size();
        first.assign(n, -1); depth.assign(n, 0); rootDistance.assign(n, 0);
        vector<int> euler;
        euler.reserve(2 * n - 1);
        struct Frame { int node, parent, nextEdge; };
        vector<Frame> stack = {{0, -1, 0}};
        first[0] = 0; euler.push_back(0);
        while (!stack.empty()) {
            Frame& frame = stack.back();
            if (frame.nextEdge == (int)tree[frame.node].size()) {
                stack.pop_back();
                if (!stack.empty()) euler.push_back(stack.back().node);
                continue;
            }
            Edge edge = tree[frame.node][frame.nextEdge++];
            if (edge.to == frame.parent) continue;
            depth[edge.to] = depth[frame.node] + 1;
            rootDistance[edge.to] = rootDistance[frame.node] + edge.weight;
            stack.push_back({edge.to, frame.node, 0});
            first[edge.to] = euler.size(); euler.push_back(edge.to);
        }
        int tourSize = euler.size();
        logs.assign(tourSize + 1, 0);
        for (int i = 2; i <= tourSize; ++i) logs[i] = logs[i / 2] + 1;
        int levels = logs[tourSize] + 1;
        table.resize(levels); table[0] = move(euler);
        for (int level = 1; level < levels; ++level) {
            int length = 1 << level, half = length >> 1;
            table[level].resize(tourSize - length + 1);
            for (int i = 0; i + length <= tourSize; ++i) {
                int left = table[level - 1][i], right = table[level - 1][i + half];
                table[level][i] = depth[left] < depth[right] ? left : right;
            }
        }
    }
    int lca(int u, int v) const {
        int left = first[u], right = first[v];
        if (left > right) swap(left, right);
        int level = logs[right - left + 1];
        int a = table[level][left], b = table[level][right - (1 << level) + 1];
        return depth[a] < depth[b] ? a : b;
    }
    ll distance(int u, int v) const {
        int ancestor = lca(u, v);
        return rootDistance[u] + rootDistance[v] - 2 * rootDistance[ancestor];
    }
};
struct Diameter { int first = -1, second = -1; ll value = -1; };
struct TopNode { Diameter all, answer; }; struct Group { int centroid, start, size, offset, topIndex; };
struct CentroidData { int offset = 0; vector<int> groups; }; struct Membership { int group, index; };
int n, queryCount, globalBase;
vector<vector<Edge>> firstTree, secondTree;
vector<char> removed, active;
vector<int> subtreeSize, parentNode, memberVertex;
vector<ll> memberWeight, globalTree;
vector<Group> groups; vector<CentroidData> centroidData;
vector<vector<Membership>> memberships; vector<Diameter> groupTree; vector<TopNode> topTree;
LCA secondTreeLCA;
int addGroup(int centroid, const vector<pair<int, ll>>& vertices) {
    int groupId = groups.size(), start = memberVertex.size();
    groups.push_back({centroid, start, (int)vertices.size(), 0, (int)centroidData[centroid].groups.size()});
    centroidData[centroid].groups.push_back(groupId);
    for (int i = 0; i < (int)vertices.size(); ++i) {
        memberVertex.push_back(vertices[i].first); memberWeight.push_back(vertices[i].second);
        memberships[vertices[i].first].push_back({groupId, i});
    } return groupId;
}
void decompose(int root) {
    vector<int> vertices, stack = {root};
    parentNode[root] = -1;
    while (!stack.empty()) {
        int node = stack.back(); stack.pop_back(); vertices.push_back(node);
        for (Edge edge : firstTree[node]) if (!removed[edge.to] && edge.to != parentNode[node]) {
            parentNode[edge.to] = node; stack.push_back(edge.to);
        }
    }
    for (int i = (int)vertices.size() - 1; i >= 0; --i) {
        int node = vertices[i]; subtreeSize[node] = 1;
        for (Edge edge : firstTree[node])
            if (!removed[edge.to] && parentNode[edge.to] == node) subtreeSize[node] += subtreeSize[edge.to];
    }
    int centroid = root, componentSize = vertices.size();
    for (int node : vertices) {
        int largestPart = componentSize - subtreeSize[node];
        for (Edge edge : firstTree[node])
            if (!removed[edge.to] && parentNode[edge.to] == node) largestPart = max(largestPart, subtreeSize[edge.to]);
            if (largestPart * 2 <= componentSize) { centroid = node; break; }
    }
    removed[centroid] = true; addGroup(centroid, {{centroid, 0}});
    for (Edge firstEdge : firstTree[centroid]) if (!removed[firstEdge.to]) {
        struct State { int node, parent; ll distance; };
        vector<State> traversal = {{firstEdge.to, centroid, firstEdge.weight}};
        vector<pair<int, ll>> component;
        while (!traversal.empty()) {
            State state = traversal.back(); traversal.pop_back();
            component.push_back({state.node, state.distance});
            for (Edge edge : firstTree[state.node])
                if (!removed[edge.to] && edge.to != state.parent)
                    traversal.push_back({edge.to, state.node, state.distance + edge.weight});
        }
        addGroup(centroid, component); decompose(firstEdge.to);
    }
}
inline ll combinedDistance(int a, int b) {
    return memberWeight[a] + memberWeight[b] + secondTreeLCA.distance(memberVertex[a], memberVertex[b]);
}
Diameter mergeDiameter(const Diameter& left, const Diameter& right) {
    if (left.first < 0) return right; if (right.first < 0) return left;
    Diameter result = left.value >= right.value ? left : right;
    int leftEnds[2] = {left.first, left.second}, rightEnds[2] = {right.first, right.second};
    int leftCount = 1 + (left.first != left.second), rightCount = 1 + (right.first != right.second);
    for (int i = 0; i < leftCount; ++i) for (int j = 0; j < rightCount; ++j) {
        ll value = combinedDistance(leftEnds[i], rightEnds[j]);
        if (value > result.value) result = {leftEnds[i], rightEnds[j], value};
    }
    return result;
}
TopNode mergeTop(const TopNode& left, const TopNode& right) {
    if (left.all.first < 0) return right; if (right.all.first < 0) return left;
    TopNode result;
    result.all = left.all.value >= right.all.value ? left.all : right.all;
    if (left.answer.first < 0) result.answer = right.answer;
    else if (right.answer.first < 0) result.answer = left.answer;
    else result.answer = left.answer.value >= right.answer.value ? left.answer : right.answer;
    int leftEnds[2] = {left.all.first, left.all.second}, rightEnds[2] = {right.all.first, right.all.second};
    int leftCount = 1 + (leftEnds[0] != leftEnds[1]), rightCount = 1 + (rightEnds[0] != rightEnds[1]);
    Diameter cross;
    for (int i = 0; i < leftCount; ++i) for (int j = 0; j < rightCount; ++j) {
        ll value = combinedDistance(leftEnds[i], rightEnds[j]);
        if (value > cross.value) cross = {leftEnds[i], rightEnds[j], value};
    }
    if (cross.value > result.all.value) result.all = cross;
    if (cross.value > result.answer.value) result.answer = cross;
    return result;
}
void updateGlobal(int index, ll value) {
    int position = globalBase + index; globalTree[position] = value;
    for (position >>= 1; position; position >>= 1)
        globalTree[position] = max(globalTree[position << 1], globalTree[position << 1 | 1]);
}
void updateMembership(Membership membership, bool isActive) {
    Group& group = groups[membership.group];
    int position = group.size + membership.index, member = group.start + membership.index;
    groupTree[group.offset + position] = isActive ? Diameter{member, member, 0} : Diameter{};
    for (position >>= 1; position; position >>= 1)
        groupTree[group.offset + position] = mergeDiameter(groupTree[group.offset + (position << 1)], groupTree[group.offset + (position << 1 | 1)]);
        CentroidData& data = centroidData[group.centroid];
        position = data.groups.size() + group.topIndex;
        Diameter root = groupTree[group.offset + 1];
        topTree[data.offset + position] = root.first < 0 ? TopNode{} : TopNode{root, Diameter{}};
        for (position >>= 1; position; position >>= 1)
            topTree[data.offset + position] = mergeTop(topTree[data.offset + (position << 1)], topTree[data.offset + (position << 1 | 1)]);
            updateGlobal(group.centroid, topTree[data.offset + 1].answer.value);
}
int main() {
    input.read(n); input.read(queryCount); firstTree.resize(n); secondTree.resize(n);
    for (int treeIndex = 0; treeIndex < 2; ++treeIndex) for (int i = 1; i < n; ++i) {
        int u, v; ll weight; input.read(u); input.read(v); input.read(weight); --u; --v;
        vector<vector<Edge>>& tree = treeIndex ? secondTree : firstTree;
        tree[u].push_back({v, weight}); tree[v].push_back({u, weight});
    }
    secondTreeLCA = LCA(secondTree);
    removed.assign(n, false); active.assign(n, false); subtreeSize.resize(n); parentNode.resize(n);
    centroidData.resize(n); memberships.resize(n);
    for (auto& list : memberships) list.reserve(18);
    groups.reserve(2 * n); memberVertex.reserve(18 * n); memberWeight.reserve(18 * n); decompose(0);
    int totalSize = 0;
    for (Group& group : groups) group.offset = totalSize, totalSize += 2 * group.size;
    groupTree.resize(totalSize); totalSize = 0;
    for (CentroidData& data : centroidData) data.offset = totalSize, totalSize += 2 * data.groups.size();
    topTree.resize(totalSize); globalBase = n; globalTree.assign(2 * n, -1);
    int activeCount = 0; string output; output.reserve(16 * queryCount);
    while (queryCount--) {
        int vertex; input.read(vertex); --vertex; active[vertex] ^= 1; activeCount += active[vertex] ? 1 : -1;
        for (Membership membership : memberships[vertex]) updateMembership(membership, active[vertex]);
        output += activeCount < 2 ? "-1\n" : to_string(globalTree[1]) + "\n";
    }
    fwrite(output.data(), 1, output.size(), stdout);
}