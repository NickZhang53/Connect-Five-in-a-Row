#include "gamescene.h"

#include <stdlib.h>
#include <time.h>

#include <QDebug>
#include <QLabel>
#include <QMessageBox>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QWidget>
#include <algorithm>
#include <vector>

#include "Board.h"
#include "ui_gamescene.h"

#define UNIT 74.5
#define LINE 5
#define WIDTH 15

#define EMPTY 0
#define COMPUTER 1
#define PLAYER 2

const int INF = 0x3f3f3f3f;
const int maxScore = 1e7;
const int minScore = -1e7;

GameScene::GameScene(int d, int fp, QWidget *parent)
    : QWidget(parent), ui(new Ui::GameScene) {
    ui->setupUi(this);
    this->setFixedSize(1916, 1397);
    this->setWindowTitle("欢乐五子棋");
    this->setWindowIcon(QPixmap(":/Image/appicon.png"));

    timer = 0, timeUsed = 0;
    difficulty = d, firstPlayer = fp, playerMoveCnt = 0;
    selecting = false, inGame = true;

    // lastPlaced = new QLabel(this);

    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < WIDTH; j++) {
            cellBtn[i][j] = new QPushButton;
            cellBtn[i][j]->setParent(this);
            cellBtn[i][j]->setGeometry(0, 0, 69, 69);
            cellBtn[i][j]->move(96 + i * UNIT + 3, 93 + j * UNIT + 3);
            cellBtn[i][j]->setStyleSheet("QPushButton{border:0px;}");
            cellBtn[i][j]->setCursor(Qt::ClosedHandCursor);
            connect(cellBtn[i][j], &QPushButton::pressed, [=]() {
                if (selecting && inGame) {
                    if (!gameBoard.board[i][j]) {
                        if (!gameBoard.prevMoves.empty())
                            changeLastMoveIconToNormal();
                        gameBoard.board[i][j] = PLAYER;
                        gameBoard.updateScore(Point(i, j));
                        placeChess(Point(i, j));
                        selecting = false;
                        playerMoveCnt++;
                    }
                }
            });
            connect(cellBtn[i][j], &QPushButton::released, [=]() {
                if (!inGame) return;
                if (gameBoard.terminate()) {
                    // qDebug() << "你赢了！";
                    inGame = false;
                    ui->gameEndLabel->setText("恭喜你赢啦！");
                    ui->goBackBtn->setText("返回主界面再来一局");
                    QPixmap memePic;
                    memePic.load(":/Image/winMeme.png");
                    ui->meme->setPixmap(memePic);
                } else if (!selecting) {
                    AIMakeMove();
                    ui->recordTime->setText(
                        QString::number((double)timeUsed / 1000.0));
                    if (gameBoard.terminate()) {
                        // qDebug() << "AI wins!";
                        ui->gameEndLabel->setText("电脑赢啦");
                        QPixmap memePic;
                        memePic.load(":/Image/LOL.png");
                        ui->meme->setPixmap(memePic);
                        inGame = false;
                        ui->goBackBtn->setText("返回主界面再来一局");
                    } else
                        selecting = true;
                }
            });
        }
    }
    connect(ui->goBackBtn, &QPushButton::clicked, [=]() {
        if (inGame && selecting) {
            if (QMessageBox::Yes ==
                QMessageBox::question(this, "退出提示", "请问您真的要退出吗？",
                                      QMessageBox::Yes | QMessageBox::No,
                                      QMessageBox::Yes)) {
                cleanUp();
                this->hide();
                emit goBack();
            }
        } else if (!inGame) {
            cleanUp();
            this->hide();
            emit goBack();
        }
    });

    connect(ui->cheatBtn, &QPushButton::clicked, [=]() {  // 悔棋
        if (!inGame) return;
        if (playerMoveCnt <= 0) { /*qDebug() << "无棋可悔";*/
            return;
        }
        if (gameBoard.prevMoves.empty()) { /*qDebug() << "无棋可悔";*/
            return;
        }
        if (!selecting) return;

        // Go back two steps
        // First
        Point last = gameBoard.prevMoves.top();
        gameBoard.board[last.row][last.col] = EMPTY;
        gameBoard.updateScore(last);

        cellBtn[last.row][last.col]->setIcon(QIcon());
        cellBtn[last.row][last.col]->setCursor(Qt::ClosedHandCursor);

        // Second
        last = gameBoard.prevMoves.top();
        gameBoard.board[last.row][last.col] = EMPTY;
        gameBoard.updateScore(last);

        cellBtn[last.row][last.col]->setIcon(QIcon());
        cellBtn[last.row][last.col]->setCursor(Qt::ClosedHandCursor);

        if (!gameBoard.prevMoves.empty()) changeLastMoveIconToBoxed();
        ui->infoText->setText("");
    });
}

GameScene::~GameScene() { delete ui; }

void GameScene::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    QPixmap pix;
    pix.load(":/Image/gamepage.png");
    painter.drawPixmap(0, 0, this->width(), this->height(), pix);
    QPen pen;
    pen.setWidth(5);
    painter.setPen(pen);
    int startX = 96, startY = 93;
    for (int i = 1; i < WIDTH; i++)
        painter.drawLine(startX, i * UNIT + startY, startX + 15 * UNIT - 3,
                         i * UNIT + startY);
    for (int i = 1; i < WIDTH; i++)
        painter.drawLine(startX + i * UNIT, startY, startX + i * UNIT,
                         startY + 15 * UNIT + 2);
}

void GameScene::changeLastMoveIconToNormal() {
    Point last = gameBoard.getLastMove();
    QPixmap chess;
    if (gameBoard.board[last.row][last.col] == COMPUTER)
        chess.load(":/Image/cross.png");
    cellBtn[last.row][last.col]->setIcon(chess);
}

void GameScene::changeLastMoveIconToBoxed() {
    Point last = gameBoard.getLastMove();
    QPixmap chess;
    if (gameBoard.board[last.row][last.col] == COMPUTER)
        chess.load(":/Image/lastPlacedCross.png");
    cellBtn[last.row][last.col]->setIcon(chess);
}

void GameScene::placeChess(Point move) {
    QPixmap chess;
    if (gameBoard.board[move.row][move.col] == COMPUTER)
        chess.load(":/Image/lastPlacedCross.png");
    else if (gameBoard.board[move.row][move.col] == PLAYER)
        chess.load(":/Image/circle.png");
    // else {qDebug() << "placeCess ERROR"; return;}
    chess.scaled(69, 69);
    cellBtn[move.row][move.col]->setIcon(chess);
    cellBtn[move.row][move.col]->setIconSize(
        QSize(chess.width(), chess.height()));
    cellBtn[move.row][move.col]->setCursor(Qt::ForbiddenCursor);
}

pair<bool, Point> GameScene::searchKill(int lim, int depth) {
    if (gameBoard.terminate())
        return make_pair(!(depth & 1), gameBoard.getLastMove());
    if (depth == lim) return make_pair(false, gameBoard.getLastMove());
    if (depth & 1) {  // max层
        // 寻找杀棋点
        gameBoard.getSuccessors();
        vector<Point> tmp = gameBoard.nxtMoves;
        for (Point nxt : tmp) {
            if (nxt.score < 720) break;
            // if (depth==1) cout << "Searching Point(" << nxt.row << "," <<
            // nxt.col << ")\n";
            gameBoard.board[nxt.row][nxt.col] = COMPUTER;
            gameBoard.updateScore(nxt);
            pair<bool, Point> res = searchKill(lim, depth + 1);
            gameBoard.board[nxt.row][nxt.col] = EMPTY;
            gameBoard.updateScore(nxt);
            if (res.first) return make_pair(res.first, nxt);
        }
        tmp.clear();
        tmp.shrink_to_fit();
        return make_pair(false, Point(-1, -1));
    } else {  // min层
        gameBoard.getSuccessors();
        Point nxt = gameBoard.nxtMoves[0];
        gameBoard.board[nxt.row][nxt.col] = PLAYER;
        gameBoard.updateScore(nxt);
        pair<bool, Point> res = searchKill(lim, depth + 1);
        gameBoard.board[nxt.row][nxt.col] = EMPTY;
        gameBoard.updateScore(nxt);
        return make_pair(res.first, nxt);
    }
}

pair<bool, Point> GameScene::searchKillMax(int lim, int depth) {  // true代表可以找到杀棋
    if (gameBoard.terminate()) return make_pair(false, Point(-1, -1));
    if (depth >= lim) return make_pair(false, Point(-1, -1));
    gameBoard.getSuccessors();
    vector<Point> tmp = gameBoard.nxtMoves;
    for (Point nxt : tmp) {
        if (nxt.score < 1000) break;
        gameBoard.board[nxt.row][nxt.col] = COMPUTER;
        gameBoard.updateScore(nxt);
        pair<bool, Point> res = searchKillMin(lim, depth + 1);
        gameBoard.board[nxt.row][nxt.col] = EMPTY;
        gameBoard.updateScore(nxt);
        if (res.first)
            return make_pair(res.first, nxt);  // 如果对方防不住，那(nxt)这步就是杀棋
    }
    tmp.clear();
    tmp.shrink_to_fit();
    return make_pair(false, Point(-1, -1));  // 对方都防住了，算杀失败
}

pair<bool, Point> GameScene::searchKillMin(int lim, int depth) {  // true代表玩家防守**不**住
    if (gameBoard.terminate()) return make_pair(true, Point(-1, -1));
    if (depth >= lim)
        return make_pair(false, Point(-1, -1));  // 到达深度，防守住了
    gameBoard.getSuccessors();
    vector<Point> tmp = gameBoard.nxtMoves;
    for (Point nxt : tmp) {
        if (nxt.score < 1200) break;
        gameBoard.board[nxt.row][nxt.col] = PLAYER;
        gameBoard.updateScore(nxt);
        pair<bool, Point> res = searchKillMax(lim, depth + 1);
        gameBoard.board[nxt.row][nxt.col] = EMPTY;
        gameBoard.updateScore(nxt);
        if (!res.first)
            return make_pair(res.first, nxt);  // 走这一步电脑不会有杀棋
    }
    tmp.clear();
    tmp.shrink_to_fit();
    return make_pair(true, Point(-1, -1));  // 电脑怎么着都会有杀棋
}

pair<int, Point> GameScene::AISearchMax(int lim, int depth,
                                        int alpha, int beta) {
    if (gameBoard.terminate())
        return make_pair(minScore + depth, gameBoard.getLastMove());
    int value = -INF, cntNxt = 0, curLim = lim;
    gameBoard.getSuccessors();

    Point ans;
    vector<Point> tmp = gameBoard.nxtMoves;
    for (Point nxt : tmp) {        // traverse all candidate points
        if (++cntNxt > 10) break;  // 最多查10个点

        gameBoard.board[nxt.row][nxt.col] = COMPUTER;  // place the chess
        gameBoard.updateScore(nxt);  // update the score based on the chess just placed

        pair<int, Point> nxtVal = AISearchMin(curLim, depth + 1, alpha, beta);  // recurse

        gameBoard.board[nxt.row][nxt.col] = EMPTY;  // backtrack
        gameBoard.updateScore(nxt);                 // backtrack score

        if (nxtVal.first > value) {
            value = nxtVal.first;
            ans = nxt;
        }
        if (value >= beta) 
            return make_pair(value, ans);
        alpha = max(alpha, value);  // alpha和value永远是相等的

        if (nxtVal.first >= maxScore - 10) 
            curLim = maxScore - nxtVal.first;
    }
    tmp.clear();
    tmp.shrink_to_fit();
    return make_pair(value, ans);
}

pair<int, Point> GameScene::AISearchMin(int lim, int depth,
                                        int alpha, int beta) {
    if (gameBoard.terminate())
        return make_pair(maxScore - depth, gameBoard.getLastMove());
    if (depth >= lim)
        return make_pair(gameBoard.evaluateGlobal(), gameBoard.getLastMove());
    int value = INF, cntNxt = 0, curLim = lim;
    gameBoard.getSuccessors();

    Point ans;
    vector<Point> tmp = gameBoard.nxtMoves;
    for (Point nxt : tmp) {
        if (++cntNxt > 10) break;
        gameBoard.board[nxt.row][nxt.col] = PLAYER;
        gameBoard.updateScore(nxt);

        pair<int, Point> nxtVal = AISearchMax(curLim, depth + 1, alpha, beta);

        gameBoard.board[nxt.row][nxt.col] = EMPTY;
        gameBoard.updateScore(nxt);

        if (nxtVal.first < value) value = nxtVal.first, ans = nxt;
        if (value <= alpha) return make_pair(value, ans);
        beta = min(beta, value);  // beta和value永远是相等的

        if (nxtVal.first <= minScore + 10) curLim = nxtVal.first - minScore;
    }
    tmp.clear();
    tmp.shrink_to_fit();
    return make_pair(value, ans);
}

void GameScene::AIMakeFirstMove() {
    srand((unsigned)time(NULL));
    Point move = firstMoves[rand() % 16];
    gameBoard.board[move.row][move.col] = COMPUTER;
    gameBoard.updateScore(move);
    placeChess(move);
}

void GameScene::AIMakeMove() {
    timer = 0, timeUsed = 0;
    //    if (difficulty >= 3){
    //        timer = clock();
    //        pair<bool, Point> killRes = searchKillMax(16,1);
    //        timeUsed = clock()-timer;
    //        qDebug() << "killSearch OK" << (double)timeUsed/1000.0;
    //        if (killRes.first){
    //            gameBoard.board[killRes.second.row][killRes.second.col] =
    //            COMPUTER; gameBoard.updateScore(killRes.second);
    //            placeChess(killRes.second);
    //            ui->infoText->setText("哈哈哈那你没了");
    //            return;
    //        }
    //    }
    //    if (ui->infoText->text() == "哈哈哈那你没了")
    //        ui->infoText->setText("哎哟不错哦");
    timer = clock();
    int searchDepth = 2 * difficulty + 2;
    // qDebug() << "searchDepth is" << searchDepth;
    Point move = AISearchMax(searchDepth, 1, -INF, INF).second;
    // qDebug() << "AISearch OK";
    timeUsed += clock() - timer;
    gameBoard.board[move.row][move.col] = COMPUTER;
    gameBoard.updateScore(move);
    placeChess(move);
}

void GameScene::startGame() {
    // qDebug() << "游戏开始啦";
    if (difficulty == 1)
        ui->difficultyRecord->setText("简单");
    else if (difficulty == 2)
        ui->difficultyRecord->setText("简单+/中等");
    else if (difficulty == 3)
        ui->difficultyRecord->setText("中等+/困难-");
    else if (difficulty == 4)
        ui->difficultyRecord->setText("困难");
    // else qDebug() << "difficulty ERROR" << difficulty;
    if (firstPlayer == PLAYER)
        gameBoard.setLastPlayer(COMPUTER);
    else {
        gameBoard.setLastPlayer(PLAYER);
        AIMakeFirstMove();
    }
    selecting = true;
}

void GameScene::cleanUp() {
    gameBoard.clearState();
    selecting = false, inGame = true;
    timer = 0, timeUsed = 0;
    ui->recordTime->setText(QString::number(0));
    ui->infoText->setText("");
    ui->gameEndLabel->setText("");
    ui->meme->setPixmap(QPixmap());
    ui->goBackBtn->setText("返回重新选择模式");
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < WIDTH; j++) {
            cellBtn[i][j]->setIcon(QIcon());
            cellBtn[i][j]->setCursor(Qt::ClosedHandCursor);
        }
    }
}
