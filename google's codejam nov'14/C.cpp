#include <iostream>
#include <map>
#include <vector>
#include <stack>
#include <queue>
#include <set>
#include <deque>
#include <cstdlib>
#include <sstream>
#include <new>
#include <climits>
#include <algorithm>
#include <string>

using namespace std;

void input_tckts(map<string, string> &mymap, int n) {
  string s1, s2;
  for(int i = 0; i < n; ++i) {
    cin >> s1 >> s2;
    mymap.insert(pair<string,string>(s1,s2));
  }
}

string get_start(map<string,string> &mymap) {
  set<string> myset;
  string ret, s;
  for(map<string,string>::iterator it = mymap.begin(); it != mymap.end(); ++it) {
    if(myset.find(it->first) != myset.end())
      continue;
    ret = it->first;
    s = it->second;
    myset.insert(ret);
    while(1) {
      if(myset.find(s) != myset.end())
        break;
      myset.insert(s);
      if(mymap.find(s) != mymap.end()) {
        s = mymap.find(s)->second;
      }
      else
        break;
    }
  }
  return ret;
}

void print_pairs(map<string,string> &mymap, string s) {
  map<string,string>::iterator it;
  while(1) {
    it = mymap.find(s);
    if(it == mymap.end()) break;
    cout << s << "-" << it->second << " ";
    s = it->second;
  }
}

int main(int argc, char **argv)
{
  int t, n;
  int idx = 1;
  cin >> t;
  map<string,string> mymap;
  //  t = 1;
  while(idx <= t) {
    cin >> n;
    mymap.clear();
    input_tckts(mymap, n);
    string s = get_start(mymap);
    cout << "Case #" << idx << ": ";
    print_pairs(mymap, s);
    cout << endl;
    idx++;
  }
  return 0;
}
