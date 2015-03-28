#include <iostream>
#include <vector>
#include "point.h"

#define MAIN_DEBUG

using namespace std;

int main()
{
  int n; cin >> n;
  vector<point> myvector(n);
  double x, y;
  for(int i = 0; i < n; ++i) {
    cin >> x >> y;
    myvector[i] = point(x,y);
  }

  int m = 2;

  while(chans(m,myvector) == false) {
    cout << "m = " << m << ". Chans algorithm failed!" << endl;
    m *= m;
  }
  cout << "m = " << m << ". Chans algorithm succeeded!" << endl;

  return 0;
}
