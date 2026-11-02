"""
Problem: Even Signal Windows
Competition: IEEE Xtreme 20
Score: 51.88 / 51.88 (100%)
"""

import sys

N,Q = input().split()
S = input()

myList = []
mySublistIntervals = []

for i in range(int(Q)):
    x,y = input().split()
    mySublistIntervals.append((int(x), int(y)))
    
prefix = [0] * (int(N)+1)

for i in range(int(N)):
    prefix[i+1] =prefix[i] ^ int(S[i])
    
count = [[0,0] for _ in range(int(N) + 2)]
    
for i in range(int(N) +1):
    count[i+1][0] = count[i][0]
    count[i+1][1] = count[i][1]
    count[i+1][prefix[i]] +=1
    
for i in range(int(Q)):
    l,r = mySublistIntervals[i]
    
    even = count[r+1][0] - count[l-1][0]
    odd = count[r+1][1] - count[l-1][1]
    
    print(even*(even-1) // 2+ odd*(odd-1)//2)