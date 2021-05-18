#include "Board.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <vector>
using namespace std;

#define WIDTH 15

#define VERTICAL 0
#define HORIZONTAL 1
#define POS_DIAG 2
#define NEG_DIAG 3

#define EMPTY 0
#define COMPUTER 1
#define PLAYER 2

#define lastMove (prevMoves.top())

int dx[4] = {-1, 0, -1, -1};
int dy[4] = {0, 1, 1, -1};
// 上，右，右斜上，左斜上

State gameBoard;

static int opponent(int p) { 
    return ((p - 1) ^ 1) + 1;
}

static int getPosDiagIdx(const Point p) { 
    return 28 - (p.row + p.col); 
}

static int getNegDiagIdx(const Point p) { 
    return 14 - (p.row - p.col); 
}

State::State() {
    lastPlayer = EMPTY;
    memset(lastScore, 0, sizeof(lastScore));
    nxtMoves.clear();
    memset(board, 0, sizeof(board));
    this->AC = new AC_Search;
}

State::~State() {
    delete AC;
    AC = nullptr;
}

int State::cntChess(int dir) {
    if (prevMoves.empty()) {
        cout << "ERROR: prevMoves empty\n";
        return 0;
    }
    int ans = 1;
    for (int d = 1; d <= 4; d++) {
        int nxtRow = lastMove.row + d * dx[dir];
        int nxtCol = lastMove.col + d * dy[dir];
        if (nxtRow < 0 || nxtRow >= WIDTH) break;
        if (nxtCol < 0 || nxtCol >= WIDTH) break;
        if (board[nxtRow][nxtCol] != lastPlayer) break;
        ans++;
    }
    for (int d = -1; d >= -4; d--) {
        int nxtRow = lastMove.row + d * dx[dir];
        int nxtCol = lastMove.col + d * dy[dir];
        if (nxtRow < 0 || nxtRow >= WIDTH) break;
        if (nxtCol < 0 || nxtCol >= WIDTH) break;
        if (board[nxtRow][nxtCol] != lastPlayer) break;
        ans++;
    }
    return ans;
}

Point State::getLastMove() {
    if (prevMoves.empty())
        return Point(-1, -1);
    else
        return prevMoves.top();
}

int State::getLastPlayer() { return lastPlayer; }

void State::setLastPlayer(int player) { lastPlayer = player; }

//! UPDATED
bool State::terminate() {
    for (int i = 0; i < 4; i++)
        if (cntChess(i) >= 5) return true;
    return false;
}

//! UPDATED
int State::evaluatePoint(Point p) {  //! 启发函数
    // 不需要先把这个点填入棋盘，即可作为预判函数
    // bool isKillPoint = false;
    int x = p.row, y = p.col, ans = 0;
    // 往每个方向走5格
    // 0表示空位，1是自己人，2是对手
    // 上，右，右斜上，左斜上
    string computerLine[4], playerLine[4];
    for (int dir = 0; dir < 4; dir++) {
        for (int step = -5; step <= 5; step++) {
            int nxtRow = x + step * dx[dir];
            int nxtCol = y + step * dy[dir];
            if (nxtRow < 0 || nxtRow >= WIDTH) continue;
            if (nxtCol < 0 || nxtCol >= WIDTH) continue;
            if (step != 0) {
                int ch = board[nxtRow][nxtCol];
                computerLine[dir] += ch + '0';
                playerLine[dir] += ch ? opponent(ch) + '0' : '0';
            } else {
                computerLine[dir] += '1';
                playerLine[dir] += '1';
            }
        }
        int compRes = AC->query(&computerLine[dir][0]);
        int playerRes = AC->query(&playerLine[dir][0]);
        ans += compRes;
        ans += playerRes;
    }
    return ans;
}

//! UPDATED
int State::evaluateGlobal() {
    // 根据lastScore相加即可
    int ans = 0;
    for (int dir = 0; dir < 2; dir++)
        for (int i = 0; i < WIDTH; i++) 
            ans += lastScore[dir][i];

    for (int dir = 2; dir < 4; dir++)
        for (int i = 0; i < 29; i++) 
            ans += lastScore[dir][i];
    return ans;
}

//! UPDATED
// bool cmp(Point a, Point b) {
//     return gameBoard.evaluatePoint(a) >= gameBoard.evaluatePoint(b);
// }

void State::getSuccessors() {
    nxtMoves.clear();
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (board[i][j]) continue;  // 非空
            // 上下左右对角有子
            if (i >= 1 && board[i - 1][j])
                nxtMoves.push_back(Point(i, j));
            else if (j >= 1 && board[i][j - 1])
                nxtMoves.push_back(Point(i, j));
            else if (i + 1 < WIDTH && board[i + 1][j])
                nxtMoves.push_back(Point(i, j));
            else if (j + 1 < WIDTH && board[i][j + 1])
                nxtMoves.push_back(Point(i, j));
            else if (i >= 1 && j >= 1 && board[i - 1][j - 1])
                nxtMoves.push_back(Point(i, j));
            else if (i + 1 < WIDTH && j >= 1 && board[i + 1][j - 1])
                nxtMoves.push_back(Point(i, j));
            else if (i >= 1 && j + 1 < WIDTH && board[i - 1][j + 1])
                nxtMoves.push_back(Point(i, j));
            else if (i + 1 < WIDTH && j + 1 < WIDTH && board[i + 1][j + 1])
                nxtMoves.push_back(Point(i, j));
        }
    }
    for (Point &p : nxtMoves) {
        p.score = evaluatePoint(p);
    }
    sort(nxtMoves.begin(), nxtMoves.end());
}

//! UPDATED
void State::updateScore(Point nxt) {  //! 调用前先放子!
    int x = nxt.row, y = nxt.col;

    if (board[x][y])
        prevMoves.push(nxt);  // place chess
    else if (prevMoves.top() == nxt)
        prevMoves.pop();  // clear chess
    else {
        cout << "updateScore ERROR\n";
        return;
    }

    lastPlayer = opponent(lastPlayer);

    // 计算受影响到的横竖撇捺的新分数
    string computerLine[4], playerLine[4];
    int computerScore[4], playerScore[4];

    // 获取一整列数据
    for (int i = 0; i < WIDTH; i++) {
        int ch = board[i][y];
        computerLine[VERTICAL] += ch + '0';
        playerLine[VERTICAL] += ch ? opponent(ch) + '0' : '0';
    }
    for (int i = 0; i < WIDTH; i++) {
        int ch = board[x][i];
        computerLine[HORIZONTAL] += ch + '0';
        playerLine[HORIZONTAL] += ch ? opponent(ch) + '0' : '0';
    }
    for (int i = min(x + y, 14), j = max(0, x + y - 14); i >= 0 && j < WIDTH;
         i--, j++) {
        int ch = board[i][j];
        computerLine[POS_DIAG] += ch + '0';
        playerLine[POS_DIAG] += ch ? opponent(ch) + '0' : '0';
    }
    for (int i = max(0, x - y), j = max(0, y - x); i < WIDTH && j < WIDTH;
         i++, j++) {
        int ch = board[i][j];
        computerLine[NEG_DIAG] += ch + '0';
        playerLine[NEG_DIAG] += ch ? opponent(ch) + '0' : '0';
    }

    // 结算分数
    for (int dir = 0; dir < 4; dir++) {
        // calculate score
        computerScore[dir] = AC->query(&computerLine[dir][0]);
        playerScore[dir] = AC->query(&playerLine[dir][0]);

        // score difference between computer and player
        int ans = computerScore[dir] - playerScore[dir];

        // update lastScore in State
        if (dir == 0)
            lastScore[dir][y] = ans;
        else if (dir == 1)
            lastScore[dir][x] = ans;
        else if (dir == 2)
            lastScore[dir][getPosDiagIdx(nxt)] = ans;
        else if (dir == 3)
            lastScore[dir][getNegDiagIdx(nxt)] = ans;
    }
}

void State::clearState() {
    lastPlayer = EMPTY;
    memset(lastScore, 0, sizeof(lastScore));
    while (!prevMoves.empty()) prevMoves.pop();
    nxtMoves.clear();
    memset(board, 0, sizeof(board));
}

void State::printBoard(Point selection) {
    cout << "   ";
    for (int i = 0; i < WIDTH; i++) {
        if (i < 10)
            cout << " " << i << " ";
        else
            cout << i << " ";
    }
    cout << "\n";
    for (int i = 0; i < WIDTH; i++) {
        if (i < 10)
            cout << i << "  ";
        else
            cout << i << " ";
        for (int j = 0; j < WIDTH; j++) {
            if (i == selection.row && j == selection.col)
                cout << ">" << board[i][j] << " ";
            else
                cout << " " << board[i][j] << " ";
        }
        cout << endl;
    }
    cout << "\n";
}
