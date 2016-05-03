#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <string>
#include <set>
#include <vector>
#include <algorithm>
 
using namespace std;

#define maxvalue 100000
int A[1010][1010];
int B[1010];

void init() {
  for(int i = 0;i < 1010;i++) {
    for(int j = 0;j < 1010;j++)
	A[i][j] = -1;
    B[i] = -1;
  }
}

// int duichen(int i,int j) {
//   int x = A[i][j];
//   int y = A[j][i];

//   if(x == -1 && y == -1)
//     return -1;
//   else if(x == -1)
//     return y;
//   else if(y == -1)
//     return x;
//   else
//     return min(x,y);
// }

int dij(int n,int src,int dst) {
  vector<int> orign;
  int cur_dis,cur_node;
  int start;
  vector<int>::iterator it;
  
  for(int i = 1;i <= n;i++)
    if(i != src)
      orign.push_back(i);
  
  cur_dis = 0;
  cur_node = src;
  start = src;
  it = orign.begin();

  while(orign.size() != 0) {
    it = orign.begin();
    while(it != orign.end()) {
      int node = *it;
      if(A[start][node] != -1)
	if(B[node] == -1)
	  B[node] = A[start][node] + cur_dis;
	else if(A[start][node] + cur_dis < B[node])
	  B[node] = A[start][node] + cur_dis;
      it++;
    }

    it = orign.begin();
    int temp;
    while(B[*it] == -1)
      it++;
    temp = B[*it];
    cur_node = *it;

    while(it != orign.end()) {
      if(B[*it] < temp && B[*it] != -1) {
	temp = B[*it];
	cur_node = *it;
      }
      it++;
    }
    cur_dis = temp; 
    if(cur_node == dst) {
      return cur_dis;
    }
    start = cur_node;
    it = find(orign.begin(),orign.end(),cur_node);
    orign.erase(it);
  }
  return B[dst];
}

int input(int i,int j,int temp) {
  if(A[i][j] == -1 || temp < A[i][j]) {
    A[i][j] = A[j][i] = temp;
    return 0;
  }
}

int main() {
  int N,M,S,T;
  int i;
  int fst,snd,temp;

  cin>>N>>M>>S>>T;
  init();
  for(i = 0;i < M;i++) {
    cin>>fst>>snd>>temp;
    input(fst,snd,temp);
  }
  cout<<dij(N,S,T)<<endl;
}
