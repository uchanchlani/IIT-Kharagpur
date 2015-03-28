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

int nbus[5001];

void init() {
  for(int i = 0; i <= 5000; i++) {
    nbus[i] = 0;
  }
}

void get_input(int n) {
  int a, b;
  for(int i = 0; i < n; i++) {
    cin >> a >> b;
    for(int j = a; j <=b; j++)
      nbus[j]++;
  }
}

void solve(int n) {
  int a;
  for(int i = 0; i < n; i++) {
    cin >> a;
    cout << nbus[a] << " ";
  }
}

int main(int argc, char **argv)
{
  int t, n;
  int idx = 1;
  cin >> t;
  //  t = 1;
  while(idx <= t) {
    cin >> n;
    init();
    get_input(n);
    cin >> n;
    cout << "Case #" << idx << ": ";
    solve(n);
    cout << endl;
    idx++;
  }
  return 0;
}
