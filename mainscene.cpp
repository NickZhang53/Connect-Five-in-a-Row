#include <QPainter>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include "mainscene.h"
#include "ui_mainscene.h"

// 格子 70*70
// 线宽 5


const int EMPTY = 0;
const int COMPUTER = 1;
const int PLAYER = 2;

MainScene::MainScene(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainScene) {
    ui->setupUi(this);
    this->setFixedSize(1916, 1397);
    this->setWindowTitle("欢乐五子棋");
    this->setWindowIcon(QPixmap(":/Image/appicon.png"));

    ui->startBtn->setStyleSheet("background-color: rgb(255,255,255)");

    difficulty = 1, firstPlayer = PLAYER;
    ui->playerBtn->setChecked(true);

    gameScene = new GameScene(difficulty, firstPlayer);

    connect(ui->slider, &QSlider::valueChanged, [&](int val) {
        gameScene->difficulty = val;
        // qDebug() << "difficulty changed to" << val;
    });

    connect(ui->playerBtn, &QRadioButton::toggled, [&](bool on) {
        if (on)
            gameScene->firstPlayer = PLAYER;
        else
            gameScene->firstPlayer = COMPUTER;
        // qDebug() << "First player is set to" <<
        // (gameScene->firstPlayer==PLAYER?"PLAYER":"COMPUTER");
    });

    connect(ui->compBtn, &QRadioButton::toggled, [&](bool on) {
        if (on)
            gameScene->firstPlayer = COMPUTER;
        else
            gameScene->firstPlayer = PLAYER;
        // qDebug() << "First player is set to" <<
        // (gameScene->firstPlayer==PLAYER?"PLAYER":"COMPUTER");
    });

    connect(ui->startBtn, &QPushButton::clicked, [=]() {
        this->hide();
        gameScene->show();
        gameScene->startGame();
    });

    connect(gameScene, &GameScene::goBack, [=]() { this->show(); });
}

void MainScene::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    QPixmap pix;
    pix.load(":/Image/homepage.png");
    painter.drawPixmap(0, 0, this->width(), this->height(), pix);
}

MainScene::~MainScene() { 
    delete ui; 
}
