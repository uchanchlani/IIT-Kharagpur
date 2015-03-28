#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include "point.h"
#include "line.h"

//#define MAIN_DEBUG

using namespace std;

vector<point> bentley_ottmann(const vector<lsegment>& lines)
{
  vector<point> ret;
  myheap H = myheap(lines);
  mybbst T;
  double min = numeric_limits<double>::min();
  point min_pt = point(min,min);
  lsegment min_line = lsegment(min_pt,min_pt);
  point ev, prevev = min_pt;
  while(H.isempty() == false) {
    ev = H.top();
    H.pop();
    if((ev.approx_eq(prevev)) && (ev.gettype()==prevev.gettype()) ){
      prevev = ev;
      continue;
    }
    int type = ev.gettype();
    if(type == START_PT) {
#ifdef MAIN_DEBUG
      cout << "MAIN: START_PT x=" << ev.getx() << endl;
#endif
      lsegment l = lines[ev.getid1()];
      T.insert(l, ev.getx());
      lsegment pre, suc;
      T.get_nbrs(l,pre,suc,ev.getx());
      if(min_line!=pre && isintersect(l,pre)) {
        point newev = getintersection(l,pre);
        newev.settype(INTERSECT_PT);
        newev.setid1(pre.getid());
        newev.setid2(l.getid());
        if(newev.getx() >= ev.getx())
          H.push(newev);
      }
      if(min_line!=suc && isintersect(l,suc)) {
        point newev = getintersection(l,suc);
        newev.settype(INTERSECT_PT);
        newev.setid1(l.getid());
        newev.setid2(suc.getid());
        if(newev.getx() >= ev.getx())
          H.push(newev);
      }
    }
    else if(type == END_PT) {
#ifdef MAIN_DEBUG
      cout << "MAIN: END_PT x=" << ev.getx() << endl;
#endif
      lsegment l = lines[ev.getid1()];
      lsegment pre, suc;
      T.get_nbrs(l,pre,suc,ev.getx());
      int chck = T.remove(l, ev.getx());
      if(chck == -1) {
#ifdef MAIN_DEBUG
        cout << "MAIN: Error in removing l from T in END_PT\n";
#endif
        ret.clear();
        break;
      }
      if(min_line!=suc && min_line!=pre && isintersect(pre,suc)) {
        point newev = getintersection(pre,suc);
        newev.settype(INTERSECT_PT);
        newev.setid1(pre.getid());
        newev.setid2(suc.getid());
        if(newev.getx() >= ev.getx())
          H.push(newev);
      }
    }
    else if(type == INTERSECT_PT) {
#ifdef MAIN_DEBUG
      cout << "MAIN: INTERSECT_PT x=" << ev.getx() << endl;
#endif
      ret.push_back(ev);
      lsegment l1=lines[ev.getid1()], l2=lines[ev.getid2()];
      lsegment pre,suc;

      T.get_nbrs(l1,pre,suc,ev.getx());
      if(min_line!=pre && isintersect(l2,pre)) {
        point newev = getintersection(l2,pre);
        newev.settype(INTERSECT_PT);
        newev.setid1(pre.getid());
        newev.setid2(l2.getid());
        if(newev.getx() >= ev.getx())
          H.push(newev);
      }
      if(min_line!=suc && isintersect(l2,suc)) {
        point newev = getintersection(l2,suc);
        newev.settype(INTERSECT_PT);
        newev.setid1(l2.getid());
        newev.setid2(suc.getid());
        if(newev.getx() >= ev.getx())
          H.push(newev);
      }

      T.get_nbrs(l2,pre,suc,ev.getx());
      if(min_line!=suc && isintersect(l1,suc)) {
        point newev = getintersection(l1,suc);
        newev.settype(INTERSECT_PT);
        newev.setid1(l1.getid());
        newev.setid2(suc.getid());
        if(newev.getx() >= ev.getx())
          H.push(newev);
      }
      if(min_line!=pre && isintersect(l1,pre)) {
        point newev = getintersection(l1,pre);
        newev.settype(INTERSECT_PT);
        newev.setid1(pre.getid());
        newev.setid2(l1.getid());
        if(newev.getx() >= ev.getx())
          H.push(newev);
      }

      int chck = T.remove(l2, ev.getx());
      if(chck == -1) {
#ifdef MAIN_DEBUG
        cout << "MAIN: Error in removing l2 from T in INTERSECT_PT\n";
#endif
        ret.clear();
        break;
      }
      T.insert(l2, ev.getx()+0.005);

    }
    else {
#ifdef MAIN_DEBUG
      cout << "MAIN: Error UNDEF event x=" << ev.getx() << endl;
#endif
      ret.clear();
      break;
    }
    prevev = ev;
  }
  return ret;
}

int main()
{
  int n; cin >> n;
  vector<lsegment> myvector(n);
  double x, y;
  point p1, p2;
  for(int i = 0; i < n; ++i) {
    cin >> x >> y;
    p1 = point(x,y);
    cin >> x >> y;
    p2 = point(x,y);
    myvector[i] = lsegment(p1,p2);
    myvector[i].setid(i);
  }

#ifdef MAIN_DEBUG
  for(int i = 0; i < n; ++i) {
    myvector[i].print(cout); cout << endl;
  }
#endif

  cout << "Running Bentley_Ottmann algorithm on the line segments\n";

  vector<point> int_pts = bentley_ottmann(myvector);

  ofstream out;
  out.open("output.out");
  for(vector<point>::iterator it = int_pts.begin(); it != int_pts.end(); ++it) {
    out << it->getx() << ' ' << it->gety() << endl;
  }
  out.close();

  cout << "\nWritten intersection points in \"output.out\" file\n";

  cout << "\nRunning Chan's algorithm on intersection points:\n";

  int m = 2;

  while(chans(m,int_pts) == false) {
    cout << "m = " << m << ". Chans algorithm failed!" << endl;
    m *= m;
  }
  cout << "m = " << m << ". Chans algorithm succeeded!" << endl;

  cout << "\nChans intermediate output in \"chans_intout.out\"\nChans output in \"chans_out.out\"\n";

  cout << "\nTo see the plot run this command in the gnuplot:\nplot \'temp.txt\' u 1:2:($3-$1):($4-$2) notitle with vectors, \"output.out\" using 1:2, \"chans_out.out\" using 1:2 with lines\n";

  return 0;
}
