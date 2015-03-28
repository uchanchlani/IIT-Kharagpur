#include "hplane.h"

ostream& operator<<(ostream& out, const halfplane& h)
{
  out << h._a << "x + " << h._b << "y <= " << h._c;
  return out;
}

istream& operator>>(istream& in, halfplane& h)
{
  if(in) {
    in >> h._a >> h._b >> h._c;
    if(h._b == 0.0 && h._a == 0.0) {
      cerr << "a & b both cannot be 0, setting them automatically to 1.0\n";
      h._a = 1.0;
      h._b = 1.0;
    }
  }
  return in;
}

bool halfplane::is_satisfy(float _x, float _y) const
{
  double a = this->_a, b = this->_b, c = this->_c, x = _x, y = _y;

  if((a*x + b*y - c) <= 0)
    return true;
  return false;
}

bool halfplane::is_satisfy(pair<float,float> pt) const
{
  return is_satisfy(pt.first, pt.second);
}

bool line::is_intersect(const line& l) const
{
  line a = *this, b = l;
  a.norm();
  b.norm();
  if(a._a != b._a)
    return true;

  if(a._a == 0.0) 
    if((a._c / a._m) == (b._c / b._m))
      return true;
    else 
      return false;

  if(a._m == b._m) {
    if(a._c == b._c)
      return true;
    return false;
  }
  return true;
}

pair<float,float> line::get_intersect(const line& l) const
{
  line a = *this, b = l;
  a.norm();
  b.norm();
  if(a.is_intersect(b)) {
    if(a._a == 0 && b._a == 0)
      return pair<float,float>(-a._c / a._m, 0.0);
    if(a._a == 0)
      return pair<float,float>(-a._c / a._m, b.gety(-a._c / a._m));
    if(b._a == 0)
      return pair<float,float>(-b._c / b._m, a.gety(-b._c / b._m));
    if(a._m == b._m)
      return pair<float,float>(0.0, a._c);
    float xret, yret;
    xret = (b._c - a._c) / (a._m - b._m);
    yret = xret*a._m + a._c;
    return pair<float,float>(xret,yret);
  }
  return pair<float,float>(numeric_limits<float>::max(),numeric_limits<float>::max());
}

pair<float,float> line::get_range(const halfplane& h) const
{
  line l = line(h);

  if(is_intersect(l)) {
    if(this->geta() == 0.0) {
      if((l.geta()==0.0) && (this->getc()/this->getm() == l.getc()/l.getm()))
        return pair<float, float>(-numeric_limits<float>::max(),numeric_limits<float>::max());

      double ycor = get_intersect(l).second;
      if(h.is_satisfy(this->getx(ycor + 1.0), ycor + 1.0)) {
        return pair<float, float>(ycor, numeric_limits<float>::max());
      }
      else {
        return pair<float, float>(-numeric_limits<float>::max(), ycor);
      }
    }
    else {
      if(this->_m == l._m)
        return pair<float, float>(-numeric_limits<float>::max(),numeric_limits<float>::max());
      double xcor = get_intersect(l).first;
      if(h.is_satisfy(xcor + 1.0, this->gety(xcor + 1.0))) {
        return pair<float, float>(xcor, numeric_limits<float>::max());
      }
      else {
        return pair<float, float>(-numeric_limits<float>::max(), xcor);
      }
    }
  }

  else {
    if(this->geta() == 0.0) {
      if(h.is_satisfy(-(this->_c / this->_m), 0.0))
        return pair<float, float>(-numeric_limits<float>::max(),numeric_limits<float>::max());
    }

    else if(h.is_satisfy(0.0, this->_c))
      return pair<float, float>(-numeric_limits<float>::max(),numeric_limits<float>::max());

    else
      return pair<float,float>(0.0, -1.0);
  }

  return pair<float,float>(0.0, -1.0);
}

bool is_intersect(const line& l1, const line& l2)
{
  return l1.is_intersect(l2);
}

pair<float,float> get_intersect(const line& l1, const line& l2)
{
  return l1.get_intersect(l2);
}
