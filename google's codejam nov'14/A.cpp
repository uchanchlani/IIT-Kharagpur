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

using namespace std;

int Arr[1002][1002];
int r, d;

bool isnbr(int x, int y, int s) {
  int i1 = x%s, i2 = y%s, j1 = x/s, j2 = y/s;
  if(i1==i2 && (j1-j2==1 || j2-j1==1)) return true;
  if(j1==j2 && (i1-i2==1 || i2-i1==1)) return true;
  return false;
}

void solve(int s, map<int,int> &mymap) {
  r = s*s; d = 1;
  int prev, curr = (mymap.find(s*s)->second), prev_d, curr_d = d;
  for(int i = s*s - 1; i > 0; --i) {
    prev = curr; prev_d = curr_d;
    curr = mymap.find(i)->second;
    if(isnbr(prev, curr, s)) {
      curr_d = prev_d + 1;
    }
    else {
      curr_d = 1;
    }
    if(curr_d >= d) {
      d = curr_d;
      r = i;
    }
  }
}

void input_matrix(int s, map<int,int> &mymap) {
  mymap.clear();
  for(int i = 0; i < s; i++)
    for(int j = 0; j < s; j++) {
      cin >> Arr[i][j];
      mymap.insert(pair<int,int>(Arr[i][j], i*s + j));
    }
  return;
}

int main(int argc, char **argv)
{
  int t, s;
  cin >> t;
  int idx = 1;
  map<int, int> mymap;
  //  t = 1;
  while(idx <= t) {
    cin >> s;
    input_matrix(s, mymap);
    r = -1; d = -1;
    solve(s, mymap);
    cout << "Case #" << idx << ": " << r << " " << d << endl;
    idx++;
  }
  return 0;
}
