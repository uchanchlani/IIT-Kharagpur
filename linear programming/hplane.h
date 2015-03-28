#ifndef HPLANE_H
#define HPLANE_H

#include <iostream>
#include <utility>
#include <vector>
#include <algorithm>
#include <cmath>
#include <limits>
#include <fstream>

//#define HPLANE_DEBUG

#define PI 3.14159265358 // value of pi correct upto 10 decimal places
#define DR_FACTOR PI/180 // this multiplied by degrees gives radians
#define RD_FACTOR 180/PI // this multiplied by radians gives degrees

using namespace std;

class halfplane // ax + by <= c
{
 public:
  halfplane(float a = 1.0, float b = 1.0, float c = 0.0) {
    if(b == 0.0 && a == 0.0) {
      cerr << "a & b both cannot be 0, setting it automatically to 1.0\n";
      this->_a = 1.0;
      this->_b = 1.0;
      }
    else {
      this->_a = a;
      this->_b = b;
    }
    this->_c = c;
  }
  halfplane(const halfplane& h) {
    this->_a = h._a;
    this->_b = h._b;
    this->_c = h._c;
  }
  halfplane operator=(const halfplane& h) {
    this->_a = h._a;
    this->_b = h._b;
    this->_c = h._c;
    return *this;
  }
  ~halfplane() {}

  halfplane operator-() const {return halfplane(-this->_a, -this->_b, -this->_c);}

  bool is_satisfy(float _x, float _y) const;
  bool is_satisfy(pair<float,float> pt) const;

  float geta() const {return _a;}
  float getb() const {return _b;}
  float getc() const {return _c;}


  friend ostream& operator<<(ostream& out, const halfplane& h);
  friend istream& operator>>(istream& in, halfplane& h);

 private:
  float _a, _b, _c;
};

class line // ay = mx + c
{
 public:
  line(float m = 1.0, float c = 1.0, float a = 1.0) {
    this->_a = a;
    this->_m = m;
    this->_c = c;
    norm();
  }
  line(const line& l) {
    this->_a = l._a;
    this->_m = l._m;
    this->_c = l._c;
    norm();
  }
  line(const halfplane& h) {
    /*    if(h.getb() == 0.0) {
      this->_a = 0.0;
      this->_m = h.geta();
      this->_c = -h.getc();
      return;
    }
    this->_a = 1.0;
    this->_m = - (h.geta() / h.getb());
    this->_c = h.getc() / h.getb();
    */
    this->_a = h.getb();
    this->_m = - h.geta();
    this->_c = h.getc();
    norm();
  }
  line& operator=(const line& l) {
    this->_a = l._a;
    this->_m = l._m;
    this->_c = l._c;
    norm();
    return *this;
  }
  ~line() {}

  float getm() const {return this->_m;}
  float getc() const {return this->_c;}
  float geta() const {return this->_a;}
  float getx(float y) const {
    if(this->_m == 0.0) 
      return 0.0;
    return (y*this->_a - this->_c) / this->_m;
  }
  float gety(float x) const {
    if(this->_a == 0.0)
      return 0.0;
    return (x*this->_m + this->_c) / this->_a;
  }

  bool is_intersect(const line& l) const;
  pair<float,float> get_intersect(const line& l) const;
  pair<float,float> get_range(const halfplane& h) const;

  void norm() {
    if(this->_a == 0.0 || this->_a == 1.0) return;
    this->_m /= this->_a;
    this->_c /= this->_a;
    this->_a = 1.0;
  }

 private:
  float _a, _m, _c;
};

bool is_intersect(const line& l1, const line& l2);
pair<float,float> get_intersect(const line& l1, const line& l2);


#endif
