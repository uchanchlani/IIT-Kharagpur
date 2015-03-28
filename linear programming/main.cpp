#include <iostream>
#include <utility>      // pair
#include <vector>
#include <algorithm>   // shuffle
#include <limits>      // numeric_limits
#include <fstream>
#include "hplane.h"

using namespace std;

//#define MAIN_DEBUG

double get_val(const pair<float,float>& eqn, const pair<float, float>& vals)
{
  double eq1 = eqn.first, eq2 = eqn.second, v1 = vals.first, v2 = vals.second;
  return (eq1*v1 + eq2*v2);
}

pair<pair<float,float>, bool> maximize(const pair<float,float>& eqn, const vector<halfplane>& vec)
{
  pair<pair<float,float>,bool> ret;
  float vx, vy;
  if(eqn.first < 0)
    vx = -numeric_limits<float>::max();
  else 
    vx = numeric_limits<float>::max();

  if(eqn.second < 0)
    vy = -numeric_limits<float>::max();
  else 
    vy = numeric_limits<float>::max();

  pair<float,float> v(vx, vy);
  /*  if(!vec[0].is_satisfy(v) || !vec[1].is_satisfy(v)) {
    line l1 = line(vec[0]), l2 = line(vec[1]);
    if(is_intersect(l1,l2))
      v = get_intersect(l1,l2);
    else {
      ret.second = false;
      return ret;
    }
    }*/
  for(int i = 0; i < vec.size(); ++i) {
#ifdef MAIN_DEBUG
    cout << "Point before iter: " << v.first << ' ' << v.second << endl;
#endif
    if(vec[i].is_satisfy(v))
      continue;
    line curr_line = line(vec[i]);
    float rmin = -numeric_limits<float>::max(), rmax = numeric_limits<float>::max();
    for(int j = 0; j < i; ++j) {
      pair<float,float> rng = curr_line.get_range(vec[j]);
      if(rng.second <= rng.first) {
        ret.second = false;
        return ret;
      }
      rmin = rmin > rng.first ? rmin : rng.first;
      rmax = rmax < rng.second ? rmax : rng.second;
    }
    if(rmin > rmax) {
      ret.second = false;
      return ret;
    }

    if(curr_line.geta() == 0.0) {
      v = pair<float,float>(curr_line.getx(rmin), rmin);
      if(get_val(eqn, pair<float,float>(curr_line.getx(rmax), rmax)) > get_val(eqn, pair<float,float>(curr_line.getx(rmin), rmin)))
        v = pair<float,float>(curr_line.getx(rmax), rmax);
    }
    else {
      v = pair<float,float>(rmin, curr_line.gety(rmin));
      if(get_val(eqn, pair<float,float>(rmax, curr_line.gety(rmax))) > get_val(eqn, pair<float,float>(rmin, curr_line.gety(rmin))))
        v = pair<float,float>(rmax, curr_line.gety(rmax));
    }
  }
#ifdef MAIN_DEBUG
    cout << "Final point: " << v.first << ' ' << v.second << endl;
#endif
  ret.first = v;
  ret.second = true;
  return ret;
}

int main(int argc, char *argv[])
{
  if(argc < 2) {
    cerr << "Missing filename!\n";
    return 1;
  }
  ifstream fin(argv[1]);
  int n;
  fin >> n;
  if(n<2) {
    cerr << "Enter atleast 2 halfplanes!!" << endl;
    fin.close();
    return 1;
  }
  vector<halfplane> my_hplanes = vector<halfplane>(n);
  for(int i = 0; i < n; ++i)
    fin >> my_hplanes[i];

  fin.close();

  pair<float, float> my_eqn;
  if(argc < 3) {
    float a, b;
    cout << "Enter for the linear equation \"a.x + b.y\" a followed by b: ";
    cin >> a >> b;
    my_eqn = pair<float,float>(a,b);
  }
  else {
    float a, b;
    ifstream fin1(argv[2]);
    fin1 >> a >> b;
    my_eqn = pair<float,float>(a,b);
    fin1.close();
  }

  // Random Permutation
  /*  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  shuffle(my_hplanes.begin(), my_hplanes.end(), std::default_random_engine(seed));
  */
#ifdef MAIN_DEBUG
  cout << "Halfplanes before shuffling: \n";
  for(int i = 0; i < my_hplanes.size(); ++i)
    cout << my_hplanes[i] << endl;
#endif

  random_shuffle(my_hplanes.begin(), my_hplanes.end());

#ifdef MAIN_DEBUG
  cout << "\nHalfplanes after shuffling: \n";
  for(int i = 0; i < my_hplanes.size(); ++i)
    cout << my_hplanes[i] << endl;
#endif

  pair<pair<float,float>, bool> value = maximize(my_eqn, my_hplanes);

  if(value.second == false)
    cout << "Halfplane intersection resulted in null set!\nNo value found!" << endl;
  else
    cout << "The maximum occurs at x=" << value.first.first << ", y=" << value.first.second << endl
         << "And it's value is " << get_val(my_eqn, value.first) << endl;

  return 0;
}
