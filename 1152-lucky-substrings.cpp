#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <string>
#include <set>
#include <algorithm>
 
using namespace std;
 
int main()
{
    string s, cur;
    set<string>t;
    cin>>s;
    int len=s.size();
    int i, j, k;
    int a[26];
    for(i=0; i<len; i++)
    {
        for(j=1; i+j<=len; j++)
        {
            cur = s.substr(i, j);
            //cout<<cur<<endl;
            int length=cur.size();
            memset(a,0,sizeof(a));
            for(k=0; k<length; k++)
            {
                a[cur[k]-97]=1;
            }
            int cnt=0;
            for(k=0; k<26; k++){
                if(a[k]>0)
                    cnt++;
            }
            if(cnt==1||cnt==2||cnt==3||cnt==5||cnt==8||cnt==13||cnt==21
               ||cnt==34||cnt==55||cnt==89)
               t.insert(cur);
        }
    }
    set<string>::iterator it=t.begin();
    while(it!=t.end())
    {
        cout<<*it<<endl;
        it++;
    }
    return 0;
}
