#include <iostream>
#include <ctime>
#include <cmath>
#include <fstream>
#include <cstdlib>

using namespace std;

double dist(double x, double y, double mid)
{
  double ret;
  ret = (x-mid)*(x-mid) + (y-mid)*(y-mid);
  ret = sqrt(ret);
  return ret;
}

int main(int argc, char** argv)
{
  if(argc < 3) {
    cerr << "Error: Missing file names!!" << endl;
    return 1;
  }
  srand(time(NULL));
  int n, min, max, r;
  cin >> n >> min >> max >> r;
  double mid = (min+max)/2;
  ofstream fout, fout1;
  fout.open(argv[1]);
  fout1.open(argv[2]);
  fout << n << endl;
  for(int i = 0; i < n;) {
    double x, y;
    x = ((double(rand()) / double(RAND_MAX)) * (max - min)) + min;
    y = ((double(rand()) / double(RAND_MAX)) * (max - min)) + min;
    if(dist(x,y,mid) > r) continue;
    fout << x << " " << y << endl;
    fout1 << x << " " << y << endl;
    i++;
  }
  fout.close();
  fout1.close();
  return 0;
}
