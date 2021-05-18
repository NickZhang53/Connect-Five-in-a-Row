#ifndef GAMESCENE_H
#define GAMESCENE_H

#include "Board.h"
#include <QWidget>
#include <QPushButton>
#include <time.h>

namespace Ui {
class GameScene;
}

class GameScene : public QWidget
{
    Q_OBJECT

public:
    explicit GameScene(int d, int fp, QWidget *parent = nullptr);
    ~GameScene();

    QPushButton* cellBtn[WIDTH+2][WIDTH+2];
    //QLabel* lastPlaced;

    int difficulty, firstPlayer, playerMoveCnt;
    bool selecting, inGame;

    void paintEvent(QPaintEvent*);

    //void playerMakeMove();
    void AIMakeFirstMove();
    void AIMakeMove();

    void placeChess(Point move);
    void changeLastMoveIconToNormal();
    void changeLastMoveIconToBoxed();

    void startGame();

    void cleanUp();

signals:
    void goBack();


private:
    Ui::GameScene *ui;

    Point firstMoves[16] = {{7,7},{7,7},{7,7},{7,7},{7,6},{7,6},{7,8},{7,8},{6,7},{6,7},{8,7},{8,7},{6,6},{6,8},{8,6},{8,8}};
    clock_t timer, timeUsed;

    pair<bool, Point> searchKill(int lim, int depth);
    pair<bool, Point> searchKillMax(int lim, int depth);
    pair<bool, Point> searchKillMin(int lim, int depth);
    pair<int, Point> AISearchMax(int lim, int depth, int alpha, int beta);
    pair<int, Point> AISearchMin(int lim, int depth, int alpha, int beta);
};

#endif // GAMESCENE_H
