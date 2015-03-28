#include "point.h"

double point::get_rel_angle(const point& p) const
{
  if(*this == p) return 0.0;
  double xrel = p.getx() - x;
  double yrel = p.gety() - y;
  double ret = atan2(yrel, xrel);
  ret *= RD_FACTOR;
  return ret;
}

bool point::comparepolar(const point& p1, const point& p2) const
{
  return this->get_rel_angle(p1) < this->get_rel_angle(p2);
}

point point::get_max_polar(const point& p1, const point& p2) const
{
  if(comparepolar(p1,p2))
    return p2;
  else
    return p1;
}

double point::get_rel_angle_POS(const point& p) const
{
  if(*this == p) return 0.0;
  double xrel = p.getx() - x;
  double yrel = p.gety() - y;
  double ret = atan2(yrel, xrel);
  ret *= RD_FACTOR;
  if(ret < 0) ret += 360;
  return ret;
}

bool point::comparepolar_POS(const point& p1, const point& p2) const
{
  return this->get_rel_angle(p1) < this->get_rel_angle(p2);
}

point point::get_max_polar_POS(const point& p1, const point& p2) const
{
  if(comparepolar(p1,p2))
    return p2;
  else
    return p1;
}

double get_angle(const point& p1, const point& p2)
{
  if(p1.getx()==p2.getx()) return 90;
  if(p1.gety()==p2.gety()) return 0;
  double ret = (p1.gety()-p2.gety()) / (p1.getx()-p2.getx());
  ret = atan(ret);
  ret *= RD_FACTOR;
  return ret;
}

bool comparex(const point& p1, const point& p2)
{
  return p1.getx() < p2.getx();
}

bool comparey(const point& p1, const point& p2)
{
  return p1.gety() < p2.gety();
}

vector<point> graham_scan(vector<point> &my_points)
{
  vector<point> ret;
  if(my_points.size() < 2) return ret;
  sort(my_points.begin(), my_points.end(), comparex);
  point st = *(my_points.begin()), en = *(my_points.rbegin());

#ifdef POINT_DEBUG
  cout << "POINT_DEBUG: GRAHAM_SCAN: start point: "; st.print(cout);
  cout << "POINT_DEBUG: GRAHAM_SCAN: end point: "; en.print(cout);
#endif

  ret.push_back(st);
  //  ret.push_back(*(++my_points.begin()));

#ifdef POINT_DEBUG
  cout << "POINT_DEBUG: GRAHAM_SCAN: HULL first elem: "; ret[0].print(cout);
#endif

  for(vector<point>::iterator it = ++my_points.begin(); it != my_points.end();) {
    //    if(st==(*it) || en==(*it)) continue;

    double angle1 = st.get_rel_angle(*it);
    double angle2 = (*it).get_rel_angle(en);
    point prev = *(ret.rbegin());

    if(angle2 <= angle1 || *it==en) {
      if(prev!=st) {
        point prev2 = *(++ret.rbegin());
        double curve1 = prev.get_rel_angle(*it) - prev2.get_rel_angle(prev);
        //        double curve2 = (*it).get_rel_angle(en) - prev.get_rel_angle(*it);

        if(curve1 > 0) {
          ret.pop_back();
        }
        else {
          ret.push_back(*it); ++it;
        }
      }
      else {
        ret.push_back(*it); ++it;
      }
    }
    else {
      ++it;
    }
  }

  //  ret.push_back(en);

  for(vector<point>::reverse_iterator it = ++my_points.rbegin(); it != my_points.rend();) {
    //    if(st==(*it) || en==(*it)) continue;

    double angle1 = st.get_rel_angle(*it);
    double angle2 = (*it).get_rel_angle(en);
    point prev = *(ret.rbegin());

    if(angle2 > angle1 || *it == st) {
      if(prev!=en) {
        point prev2 = *(ret.rbegin() + 1);
        double curve1 = prev.get_rel_angle(prev2) - (*it).get_rel_angle(prev);
        //        double curve2 = (*it).get_rel_angle(prev) - st.get_rel_angle(*it);

        if(curve1 < 0) {
          ret.pop_back();
        }
        else {
          ret.push_back(*it); ++it;
        }
      }
      else {
        ret.push_back(*it); ++it;
      }
    }
    else {
      ++it;
    }
  }

  ret.pop_back();
  return ret;
}

point get_left_most(const vector<point> &my_points)
{
  return point(*min_element(my_points.begin(), my_points.end(), comparex));
}

double get_eucledian(point p)
{
  return sqrt((p.getx() * p.getx()) + (p.gety() * p.gety()));
}

double get_dist(point p1, point p2)
{
  return sqrt((p1.getx() - p2.getx())*(p1.getx() - p2.getx()) + (p1.gety() - p2.gety())*(p1.gety() - p2.gety()));
}

double get_acosine(point p1, point p2)
{
  return acos(((p1.getx() * p2.getx()) + (p1.gety() * p2.gety())) / (get_eucledian(p1) * get_eucledian(p2)));
}

vector<point> jarvis_march(vector<point> &my_points)
{
  point st = get_left_most(my_points);
#ifdef POINT_DEBUG
  cout << "POINT_DEBUG: JARVIS_MARCH: "; st.print(cout);
#endif

  vector<point> ret;
  point prev = point(st.getx(), numeric_limits<double>::min()), curr = st, next;

  while(1) {
#ifdef POINT_DEBUG
    cout << "POINT_DEBUG: JARVIS_MARCH: current push is: "; curr.print(cout);
#endif

    ret.push_back(curr);
    next = point(*my_points.begin());
    double min_cos = get_acosine(next - curr, curr - prev);

    for(vector<point>::iterator it = my_points.begin(); it != my_points.end(); ++it) {
      if((*it) == curr) continue;
      double curr_cos = get_acosine((*it) - curr, curr - prev);
      if(curr_cos <= min_cos) {
        min_cos = curr_cos;
        next = (*it);
      }
    }

    prev = curr;
    curr = next;

    if(curr == st) break;
  }
    
  return ret;
}

float cross_prod(point p1, point p2, point p3)
{
	return (p2.gety() - p1.gety()) * (p3.getx() - p2.getx()) - (p2.getx() - p1.getx()) * (p3.gety() - p2.gety());
}


point binary_search(point ref, const vector<point> &my_points)
{
  if(my_points.size()==0) return ref;
  if(my_points.size()==1) return my_points[0];

  int sz = my_points.size();
  point ret = my_points[0];

  for(int i = 0; i < sz; ++i) {
    if(ref==my_points[i]) continue;
    float angle1 = cross_prod(ref,my_points[i],my_points[(i+1)%sz]);
    float angle2 = cross_prod(ref,my_points[i],my_points[(i-1)%sz]);

    if((angle1 <= 0) && (angle2 <= 0)) {
      ret = my_points[i];
      break;
    }
  }
  return ret;
}
    
point modified_jarvis(point ref, const vector<point> &my_points)
{
  if(my_points.size()==0) return ref;
  if(my_points.size()==1) return my_points[0];

  point ret = my_points[0];
  for(int i = 0; i < my_points.size(); ++i) {
    if(ref==my_points[i]) continue;

    if((cross_prod(ref, ret, my_points[i]) > 0) || ((cross_prod(ref, ret, my_points[i]) == 0) && (get_dist(ref,my_points[i]) < get_dist(ref, ret))))
      ret = my_points[i];
  }
  return ret;
}

bool chans(int m, vector<point> &my_points)
{
  vector<vector<point> > G;

  for(int i = 0; i < my_points.size(); i+=m) {
    if(i+m < my_points.size()) {
      vector<point> tempvec(my_points.begin()+i, my_points.begin()+i+m);
      G.push_back(graham_scan(tempvec));
    }
    else {
      vector<point> tempvec(my_points.begin()+i, my_points.begin()+my_points.size());
      G.push_back(graham_scan(tempvec));
    }
  }

  ofstream iout;
  iout.open("intoutput.out");

  for(int i = 0; i < G.size(); ++i) {
    for(int j = 0; j < G[i].size(); ++j) {
      iout << G[i][j].getx() << ' ' << G[i][j].gety() << endl;
    }
    iout << G[i][0].getx() << ' ' << G[i][0].gety() << endl << endl << endl;
  }
  iout.close();

  vector<point> chull;
  point ref(numeric_limits<float>::min(), 0);

  for(int i = 0; i < m; ++i) {
    vector<point> tangents;
    for(vector<vector<point> >::iterator it = G.begin(); it != G.end(); ++it) {
      tangents.push_back(binary_search(ref, *it));
    }

    ref = modified_jarvis(ref, tangents);

    if((chull.size() > 0) && (ref == chull[0])) break;

    chull.push_back(ref);
  }

  if(chull.size() >= m) return false;

  ofstream out;
  out.open("output.out");
  for(vector<point>::iterator it = chull.begin(); it != chull.end(); ++it) {
    out << it->getx() << ' ' << it->gety() << endl;
  }
  vector<point>::iterator it = chull.begin();
  out << it->getx() << ' ' << it->gety() << endl;
  out.close();

  return true;
}
