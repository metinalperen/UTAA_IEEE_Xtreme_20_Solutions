"""
Problem: Encrypted Thresholds
Competition: IEEE Xtreme 20
Score: 48.69 / 64.92 (75.00%)
"""

import sys

input = sys.stdin.readline
sys.setrecursionlimit(500000)

N,Q = map(int, input().split())

adj = [[] for _ in range(N+1)]
for _ in range(N-1):
    u,v,w = map(int,input().split())
    adj[u].append((v,w))
    adj[v].append((u,w))
    
xorList = [0]* (N+1)
stack = [(1,0,0)]
while stack:
    u,p,cur = stack.pop()
    xorList[u] = cur
    for v,w in adj[u]:
        if v != p:
            stack.append((v,u,cur^w))
    
MAX_NODES = N * 22+5
ch0 = [0] * MAX_NODES
ch1 = [0] * MAX_NODES
sz = [0] * MAX_NODES
node_count=1

for i in range(1,N+1):
    val = xorList[i]
    curr = 1
    sz[curr] +=1
    for bit in range(19,-1,-1):
        b = (val >>bit)&1
        if b == 0:
            if ch0[curr] == 0:
                node_count +=1
                ch0[curr] = node_count
            curr = ch0[curr]
        else:
            if ch1[curr] ==0:
                node_count +=1
                ch1[curr] = node_count
            curr = ch1[curr]
        sz[curr] +=1

def count_pairs(u,v,bit,k):
    if u==0 or v==0:
        return 0
    if bit<0:
        return 0

    k_bit = (k>>bit)&1    
        
    if u ==v:
        u0,u1 = ch0[u], ch1[u]
        if k_bit ==1:
            res = (sz[u0] * (sz[u0]-1)) // 2 + (sz[u1] * (sz[u1] -1))//2
            res += count_pairs(u0,u1,bit-1,k)
            return res
        else:
            return count_pairs(u0,u0,bit-1,k)+ count_pairs(u1,u1,bit-1,k)
    else:
        u0,u1 = ch0[u],ch1[u]
        v0,v1 = ch0[v], ch1[v]
        if k_bit ==1:
            res= sz[u0] * sz[v0] + sz[u1] * sz[v1]
            res += count_pairs(u0,v1,bit-1,k) + count_pairs(u1,v0,bit-1,k)
            return res
        else:
            return count_pairs(u0,v0,bit-1,k)+count_pairs(u1,v1,bit-1,k)
        
memo = {}

total_all= N*(N-1) //2
out = []

for _ in range(Q):
    k = int(input())
    if k<=0:
        out.append("0")
    elif k >=(1<<20):
        out.append(str(total_all))
    elif k in memo:
        out.append(str(memo[k]))
    else:
        ans= count_pairs(1,1,19,k)
        memo[k] = ans
        out.append(str(ans))
        
sys.stdout.write("\n".join(out) + "\n")
        