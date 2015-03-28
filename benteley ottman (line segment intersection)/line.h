#ifndef LINE_H
#define LINE_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>
#include <fstream>
#include "point.h"

#define LINE_DEBUG

//#define NULL_LINE lsegment(point(numeric_limits<double>::min()),point(numeric_limits<double>::min()))

using namespace std;

class lsegment
{
 public:
  lsegment() {st = point(); en = point();}
  lsegment(const point& p1, const point& p2);
  lsegment(const lsegment& l) {st=l.st; en=l.en; id=l.id;}
  lsegment& operator=(const lsegment& l) {st=l.st; en=l.en; id=l.id; return  *this;}
  ~lsegment() {}

  bool operator==(const lsegment& l) const {return (st==l.st)&&(en==l.en);}
  bool operator!=(const lsegment& l) const {return (st!=l.st)||(en!=l.en);}
  ostream& print(ostream &out) const {if(out) out << "start: "; st.print(out); out << "; end: "; en.print(out); return out;}
  void setid(int i) {id=i; return;}
  point getst() const {return st;}
  point geten() const {return en;}
  double getm() const {return st.get_rel_angle(en);}
  double gety(double x) const;
  int getid() const {return id;}

 private:
  point st, en;
  int id;
};

class myheap
{
 public:
  myheap() {vec.clear(); count=0;}
  myheap(const vector<lsegment>& lines);
  ~myheap() {vec.clear(); count = 0;}

  void heapify();
  void push(const point& p);
  point top();
  void pop();
  bool isempty();

 private:
  vector<point> vec;
  int count;
};

struct tree
{
  lsegment val;
  int height;
  tree *left, *right;
};

class mybbst
{
 public:
  mybbst() {root=NULL; count=0;}
  ~mybbst();

  void insert(lsegment l, double xpos);
  int remove(lsegment l, double xpos);
  void get_nbrs(lsegment l, lsegment& prev, lsegment& next, double x);

 private:
  tree *root;
  int count;
};

bool isintersect(const lsegment& l1, const lsegment& l2);
point getintersection(const lsegment& l1, const lsegment& l2);

#endif
