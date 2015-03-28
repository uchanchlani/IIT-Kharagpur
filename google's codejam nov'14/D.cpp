#include <iostream>
#include <map>
#include <vector>
#include <stack>
#include <queue>
#include <set>
#include <deque>
#include <cstdlib>
#include <sstream>
#include <new>
#include <climits>
#include <algorithm>

using namespace std;

char board[8][8];

void init() {
  for(int i = 0; i < 8; ++i)
    for(int j = 0; j < 8; ++j)
      board[i][j] = ' ';
}

void get_input(int n) {
  string s;
  int x, y;
  for(int i = 0; i < n; i++) {
    cin >> s;
    x = s[0] - 'A';
    y = s[1] - '1';
    board[x][y] = s[3];
  }
}

void print_board() {
  for(int i = 0; i < 8; ++i) {
    for(int j = 0; j < 8; ++j)
      cout << board[i][j];
    cout << endl;
  }
  cout << "\n\n";
}

char get_piece(int x, int y) {
  if(x < 0 || x > 7) return ' ';
  if(y < 0 || y > 7) return ' ';
  return board[x][y];
}

int king_count(int x, int y) {
  int ret = 0;
  if(get_piece(x-1,y-1) != ' ') ret++;
  if(get_piece(x-1,y) != ' ') ret++;
  if(get_piece(x-1,y+1) != ' ') ret++;
  if(get_piece(x,y-1) != ' ') ret++;
  if(get_piece(x,y+1) != ' ') ret++;
  if(get_piece(x+1,y-1) != ' ') ret++;
  if(get_piece(x+1,y) != ' ') ret++;
  if(get_piece(x+1,y+1) != ' ') ret++;
  return ret;
}

int rook_count(int x, int y) {
  int ret = 0;
  for(int i = x+1; i < 8; ++i) {
    if(get_piece(i,y) != ' ') {
      ret++;
      break;
    }
  }
  for(int i = x-1; i >= 0; --i) {
    if(get_piece(i,y) != ' ') {
      ret++;
      break;
    }
  }
  for(int i = y+1; i < 8; ++i) {
    if(get_piece(x,i) != ' ') {
      ret++;
      break;
    }
  }
  for(int i = y-1; i >= 0; --i) {
    if(get_piece(x,i) != ' ') {
      ret++;
      break;
    }
  }
  return ret;
}

int bishop_count(int x, int y) {
  int ret = 0;
  for(int i = x+1, j = y+1; i < 8 && j < 8; ++i, ++j) {
    if(get_piece(i,j) != ' ') {
      ret++;
      break;
    }
  }
  for(int i = x+1, j = y-1; i < 8 && j >= 0; ++i, --j) {
    if(get_piece(i,j) != ' ') {
      ret++;
      break;
    }
  }
  for(int i = x-1, j = y+1; i >= 0 && j < 8; --i, ++j) {
    if(get_piece(i,j) != ' ') {
      ret++;
      break;
    }
  }
  for(int i = x-1, j = y-1; i >= 0 && j >= 0; --i, --j) {
    if(get_piece(i,j) != ' ') {
      ret++;
      break;
    }
  }
  return ret;
}

int queen_count(int x, int y) {
  int ret = 0;
  ret += rook_count(x,y);
  ret += bishop_count(x,y);
  return ret;
}

int knight_count(int x, int y) {
  int ret = 0;
  if(get_piece(x+2,y+1) != ' ') ret++;
  if(get_piece(x+2,y-1) != ' ') ret++;
  if(get_piece(x-2,y+1) != ' ') ret++;
  if(get_piece(x-2,y-1) != ' ') ret++;
  if(get_piece(x+1,y+2) != ' ') ret++;
  if(get_piece(x-1,y+2) != ' ') ret++;
  if(get_piece(x+1,y-2) != ' ') ret++;
  if(get_piece(x-1,y-2) != ' ') ret++;
  return ret;
}

int pawn_count(int x, int y) {
  int ret = 0;
  if(get_piece(x+1,y-1) != ' ') ret++;
  if(get_piece(x+1,y+1) != ' ') ret++;

  return ret;
}

int solve() {
  int ret = 0;
  for(int i = 0; i < 8; ++i)
    for(int j = 0; j < 8; ++j) {
      if(board[i][j] == ' ') continue;
      else if(board[i][j] == 'K') ret += king_count(i,j);
      else if(board[i][j] == 'Q') ret += queen_count(i,j);
      else if(board[i][j] == 'R') ret += rook_count(i,j);
      else if(board[i][j] == 'B') ret += bishop_count(i,j);
      else if(board[i][j] == 'N') ret += knight_count(i,j);
      else if(board[i][j] == 'P') ret += pawn_count(i,j);
    }
  return ret;
}

int main(int argc, char **argv)
{
  int t, n;
  int idx = 1;
  cin >> t;
  //  t = 1;
  while(idx <= t) {
    cin >> n;
    init();
    get_input(n);
    //print_board();
    cout << "Case #" << idx << ": " << solve() << endl;
    idx++;
  }
  return 0;
}
