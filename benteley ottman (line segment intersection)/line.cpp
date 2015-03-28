#include "line.h"


// LINE SEGMENT FUNCTIONS
lsegment::lsegment(const point& p1, const point& p2)
{
  if(p1.getx() < p2.getx()) {
    st = p1; en = p2;
  }
  else {
    en = p1; st = p2;
  }
}

double lsegment::gety(double x) const
{
  double x1=st.getx(), x2=en.getx(), y1=st.gety(), y2=en.gety();
  if(x1==x2) return (y1+y2)/2;
  double y = y2-y1;
  y *= (x-x1);
  y /= (x2-x1);
  y += y1;
  return y;
}


// HEAP FUNCTIONS
myheap::myheap(const vector<lsegment>& lines)
{
  vec.clear();
  count = 2*lines.size();
  vec = vector<point>(count);
  for(int i = 0; i < lines.size(); ++i) {
    vec[2*i] = lines[i].getst();
    vec[2*i].settype(START_PT);
    vec[2*i].setid1(i);

    vec[2*i + 1] = lines[i].geten();
    vec[2*i + 1].settype(END_PT);
    vec[2*i + 1].setid1(i);
  }
  heapify();
}

void myheap::heapify()
{
  make_heap(vec.begin(), vec.end(), comparexrev);
}

void myheap::push(const point& p)
{
  count++;
  vec.push_back(point(p));
  push_heap(vec.begin(), vec.end(), comparexrev);
}

point myheap::top()
{
  if(count == 0)
    return point();
  return vec[0];
}

void myheap::pop()
{
  if(count == 0)
    return;
  count--;
  pop_heap(vec.begin(), vec.end(), comparexrev);
  vec.pop_back();
}

bool myheap::isempty()
{
  if(count == 0)
    return true;
  return false;
}


/*
 * AVL TREE CODE UTILITY FUNCTIONS STARTS HERE
 */
int depth(tree *N)
{
  if (N == NULL)
    return 0;

  return N->height;
}

int max_int(int a, int b)
{
  if(a > b)
    return a;
  else
    return b;
}

tree *rightr(tree *y)
{
  tree *x = y->left;
  tree *T2 = x->right;
  x->right = y;
  y->left = T2;
  y->height = max_int(depth(y->left), depth(y->right))+1;
  x->height = max_int(depth(x->left), depth(x->right))+1;
  return x;
}
 
tree *leftr(tree *x)
{
  tree *y = x->right;
  tree *T2 = y->left;
  y->left = x;
  x->right = T2;
  x->height = max_int(depth(x->left), depth(x->right))+1;
  y->height = max_int(depth(y->left), depth(y->right))+1;
  return y;
}
 
int getBalance(tree *N)
{
  if (N == NULL)
    return 0;

  return depth(N->left) - depth(N->right);
}
 
tree* insert_util(tree* node, lsegment key, double x)
{
  if (node == NULL) {
    tree *ret = new tree();
    ret->left = ret->right = NULL;
    ret->val = key;
    return ret;
  }
 
  if (key.gety(x) < node->val.gety(x))
    node->left  = insert_util(node->left, key, x);
  else
    node->right = insert_util(node->right, key, x);
 
  node->height = max_int(depth(node->left), depth(node->right)) + 1;
 
  int balance = getBalance(node);

  // Left Left Case
  if (balance > 1 && key.gety(x) < node->left->val.gety(x))
    return rightr(node);
  // Right Right Case
  if (balance < -1 && key.gety(x) > node->right->val.gety(x))
    return leftr(node);
  // Left Right Case
  if (balance > 1 && key.gety(x) > node->left->val.gety(x)) {
    node->left =  leftr(node->left);
    return rightr(node);
  }
  // Right Left Case
  if (balance < -1 && key.gety(x) < node->right->val.gety(x)) {
    node->right = rightr(node->right);
    return leftr(node);
  }
 
  return node;
}

tree * minNode(tree* node)
{
  tree* ret = node;
 
  while (ret->left != NULL)
    ret = ret->left;
 
  return ret;
}
 
tree* remove_util(tree* root, lsegment key, double x)
{
 
  if (root == NULL)
    return root;
 
  if ( key.gety(x) < root->val.gety(x) )
    root->left = remove_util(root->left, key, x);
 
  else if( key.gety(x) > root->val.gety(x) )
    root->right = remove_util(root->right, key, x);
 
  else {
    if( (root->left == NULL) || (root->right == NULL) ) {
      tree *temp = root->left ? root->left : root->right;
 
      if(temp == NULL) {
        temp = root;
        root = NULL;
      }
      else
        *root = *temp;
 
      delete temp;
    }
    else {
      tree* temp = minNode(root->right);
 
      root->val = temp->val;
 
      root->right = remove_util(root->right, temp->val, x);
    }
  }
 
  if (root == NULL)
    return root;
 
  root->height = max_int(depth(root->left), depth(root->right)) + 1;
 
  int balance = getBalance(root);
 
  // Left Left Case
  if (balance > 1 && getBalance(root->left) >= 0)
    return rightr(root);
  // Left Right Case
  if (balance > 1 && getBalance(root->left) < 0) {
    root->left =  leftr(root->left);
    return rightr(root);
  }
  // Right Right Case
  if (balance < -1 && getBalance(root->right) <= 0)
    return leftr(root);
  // Right Left Case
  if (balance < -1 && getBalance(root->right) > 0) {
    root->right = rightr(root->right);
    return leftr(root);
  }
 
  return root;
}
/*
 * AVL TREE CODE UTILITY FUNCTIONS ENDS HERE
 */


// MISC BBST CODE UTILITY FUNCTIONS STARTS HERE
void cleartree(tree *node)
{
  if(node==NULL)
    return;
  cleartree(node->left);
  cleartree(node->right);
  delete node;
}

void findPreSuc_util(tree* root, lsegment& pre, lsegment& suc, lsegment key, double x)
{
  if (root == NULL)
    return;
 
  if (root->val.gety(x) == key.gety(x)) {
    if (root->left != NULL) {
      tree* tmp = root->left;
      while (tmp->right)
        tmp = tmp->right;
      pre = tmp->val ;
    }
 
    if (root->right != NULL) {
      tree* tmp = root->right ;
      while (tmp->left)
        tmp = tmp->left ;
      suc = tmp->val ;
    }
    return ;
  }
 
  if (root->val.gety(x) > key.gety(x)) {
    suc = root->val ;
    findPreSuc_util(root->left, pre, suc, key, x) ;
  }
  else {
    pre = root->val ;
    findPreSuc_util(root->right, pre, suc, key, x) ;
  }
}


// BALANCED BINARY TREE  CLASS FUNCTIONS
mybbst::~mybbst()
{
  cleartree(root);
  root = NULL;
  count = 0;
}

void mybbst::insert(lsegment l, double xpos)
{
  root = insert_util(root,l,xpos);
  return;
}

int mybbst::remove(lsegment l, double xpos)
{
  root = remove_util(root,l,xpos);
  return 0;
}


void mybbst::get_nbrs(lsegment l, lsegment& prev, lsegment& next, double x)
{
  double min = numeric_limits<double>::min();
  point min_pt1 = point(min,min);
  point min_pt2 = point(min,min);
  prev = lsegment(min_pt1,min_pt2);
  next = lsegment(min_pt1,min_pt2);
  findPreSuc_util(root, prev, next, l, x);
}


// MISC FUNCTIONS
bool isintersect(const lsegment& l1, const lsegment& l2)
{
  if(l1==l2) return false;
  if(cross_prod(l1.getst(),l2.getst(),l1.geten()) * cross_prod(l1.getst(),l2.geten(),l1.geten()) >= 0)
    return false;
  if(cross_prod(l2.getst(),l1.getst(),l2.geten()) * cross_prod(l2.getst(),l1.geten(),l2.geten()) >= 0)
    return false;
  return true;
}

bool getintersection_util(double p0_x, double p0_y, double p1_x, double p1_y, double p2_x, double p2_y, double p3_x, double p3_y, double *i_x, double *i_y)
{
  double s02_x, s02_y, s10_x, s10_y, s32_x, s32_y, s_numer, t_numer, denom, t;
  s10_x = p1_x - p0_x;
  s10_y = p1_y - p0_y;
  s32_x = p3_x - p2_x;
  s32_y = p3_y - p2_y;

  denom = s10_x * s32_y - s32_x * s10_y;
  if (denom == 0)
    return 0;
  bool denomPositive = denom > 0;

  s02_x = p0_x - p2_x;
  s02_y = p0_y - p2_y;
  s_numer = s10_x * s02_y - s10_y * s02_x;
  if ((s_numer < 0) == denomPositive)
    return 0;

  t_numer = s32_x * s02_y - s32_y * s02_x;
  if ((t_numer < 0) == denomPositive)
    return 0;

  if (((s_numer > denom) == denomPositive) || ((t_numer > denom) == denomPositive))
    return 0;
  t = t_numer / denom;
  if (i_x != NULL)
    *i_x = p0_x + (t * s10_x);
  if (i_y != NULL)
    *i_y = p0_y + (t * s10_y);

  return 1;
}

point getintersection(const lsegment& l1, const lsegment &l2)
{
  point p0=l1.getst(),p1=l1.geten(),p2=l2.getst(),p3=l2.geten();
  double x,y;
  if(getintersection_util(p0.getx(),p0.gety(),p1.getx(),p1.gety(),p2.getx(),p2.gety(),p3.getx(),p3.gety(),&x,&y) == 0)
    return point();
  return point(x,y);
}
