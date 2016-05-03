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

class KMP {

public:
int next[10010];

int gen_next(string& s) {
  int len;
  int j,k;
  
  k = -1;
  j = 0;
  next[0] = -1;
  len = s.length();
  while(j < len) {
    if(k == -1 || s[k] == s[j]) {
      k++;
      j++;
      next[j] = k;
    } else {
      k = next[k];
    }
  }
  return 0;
}

int kmp(string& s1,int len1,string& s2,int len2) {
  int i,j;
  int count;

  count = 0;
  i = j = 0;

  if(len2 == 0)
    return 0;
  while (i < len1) {
    while(i < len1 && j < len2) {
      if(j == -1 || s1[i] == s2[j]) {
	i++;
	j++;
      } else {
	j = next[j];
      }
    }
    if(j == len2) {
      count++;
      //i = i - j + 1;
      //j = 0;
      j = next[j];
    }
    if(i == len1)
      break;
  }
  return count;
}
};

int main() {
  int n;
  string s1,s2;

  KMP ai;
  cin>>n;
  while(n > 0) {
    cin>>s2;
    cin>>s1;
    ai.gen_next(s2);
    cout<<ai.kmp(s1,s1.length(),s2,s2.length())<<endl;
    n--;
  }
  return 0;
}
