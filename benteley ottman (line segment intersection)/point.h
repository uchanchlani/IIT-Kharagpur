#ifndef POINT_H
#define POINT_H

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

#define UNDEF 0
#define START_PT 1
#define END_PT 2
#define INTERSECT_PT 3

using namespace std;

class point
{
 public:
  point() {x=0.0; y=0.0; type=UNDEF; id1=-1; id2=-1;}
  point(double x1, double y1) {x=x1; y=y1; type=UNDEF; id1=-1; id2=-1;}
  point(double x1, double y1, int tp) {x=x1; y=y1; type=tp; id1=-1; id2=-1;}
  point(const point& p) {x=p.x; y=p.y; type=p.type; id1=p.id1; id2=p.id2;}
  point& operator=(const point& p) {x=p.x; y=p.y; type=p.type; id1=p.id1; id2=p.id2; return *this;}
  ~point() {}

  bool operator==(const point& p) const {return (x==p.x)&&(y==p.y);}
  bool approx_eq(const point& p) const;
  bool operator!=(const point& p) const {return (x!=p.x)||(y!=p.y);}
  point operator-(const point& p) const {return point(this->x - p.x, this->y - p.y);}

  ostream& print(ostream &out) const {if(out) out << "x: " << x << ", y: " << y; return out;}
  void settype(int tp) {type=tp;}
  void setid1(int i) {id1=i;}
  void setid2(int i) {id2=i;}
  double getx() const {return x;}
  double gety() const {return y;}
  int gettype() const {return type;}
  int getid1() const {return id1;}
  int getid2() const {return id2;}
  double get_rel_angle(const point& p) const;
  bool comparepolar(const point& p1, const point& p2) const;
  point get_max_polar(const point& p1, const point& p2) const;

 private:
  double x, y;
  int type;
  int id1, id2;
};

double get_angle(const point& p1, const point& p2);
bool comparex(const point& p1, const point& p2);
bool comparey(const point& p1, const point& p2);
bool comparexrev(const point& p1, const point& p2);
bool compareyrev(const point& p1, const point& p2);

vector<point> graham_scan(vector<point> &my_points);
float cross_prod(point p1, point p2, point p3);
bool chans(int m, vector<point> &my_points);

#endif
