#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>
#include <fstream>

//#define POINT_DEBUG

#define PI 3.14159265358 // value of pi correct upto 10 decimal places
#define DR_FACTOR PI/180 // this multiplied by degrees gives radians
#define RD_FACTOR 180/PI // this multiplied by radians gives degrees

using namespace std;

class point
{
 public:
  point() {x=0.0; y=0.0;}
  point(double x1, double y1) {x=x1; y=y1;}
  point(const point& p) {x=p.x; y=p.y;}
  point& operator=(const point& p) {x=p.x; y=p.y; return *this;}
  ~point() {}

  bool operator==(const point& p) const {return (x==p.x)&&(y==p.y);}
  bool operator!=(const point& p) const {return (x!=p.x)||(y!=p.y);}
  point operator-(const point& p) const {return point(this->x - p.x, this->y - p.y);}

  ostream& print(ostream &out) const {if(out) out << "x: " << x << ", y: " << y << endl; return out;}
  double getx() const {return x;}
  double gety() const {return y;}
  double get_rel_angle(const point& p) const;
  bool comparepolar(const point& p1, const point& p2) const;
  point get_max_polar(const point& p1, const point& p2) const;

  double get_rel_angle_POS(const point& p) const;
  bool comparepolar_POS(const point& p1, const point& p2) const;
  point get_max_polar_POS(const point& p1, const point& p2) const;

 private:
  double x, y;
};

double get_angle(const point& p1, const point& p2);
bool comparex(const point& p1, const point& p2);
bool comparey(const point& p1, const point& p2);

vector<point> graham_scan(vector<point> &my_points);
vector<point> jarvis_march(vector<point> &my_points);
bool chans(int m, vector<point> &my_points);
