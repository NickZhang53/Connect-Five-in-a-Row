#pragma once
#include <stack>
#include <vector>

#include "pattern_search.h"
using namespace std;

#define WIDTH 15

struct Point {
    int row, col;
    int score;
    Point() { row = 0, col = 0, score = 0; }
    Point(int _row, int _col) : row(_row), col(_col), score(0) {}
    bool operator==(const Point &a) const {
        return (row == a.row) && (col == a.col);
    }
    bool operator<(const Point &a) const { 
        return a.score < score; 
    }
};

class State {
   private:
    int lastPlayer;  // 为 1电脑 2玩家
    int lastScore[4][29];
    // 电脑分数-玩家分数
    int cntChess(int dir);

   public:
    stack<Point> prevMoves;
    AC_Search *AC;
    vector<Point> nxtMoves;
    int board[WIDTH][WIDTH];
    State();
    ~State();
    Point getLastMove();
    int getLastPlayer();
    void setLastPlayer(int player);
    bool terminate();
    int evaluateGlobal();
    int evaluatePoint(Point p);  // 不需要先把这个点填入棋盘，即可作为预判函数
    void getSuccessors();
    void updateScore(Point nxt);  //! 调用前先放子!
    void printBoard(Point selection);
    void clearState();
    // bool myCmp(Point a, Point b);
};

// bool cmp(Point a, Point b);

extern State gameBoard;
