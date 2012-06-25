#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QPainter>
#include <QPoint>
#include <QMouseEvent>
#include <QDebug>
#include <QtNetwork/QTcpSocket>
#include "Images.h"
#include "Model.h"
#include "Controller.h"
#include "Defines.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void paintEvent( QPaintEvent *event );
    void mousePressEvent ( QMouseEvent * ev );

private slots:
    void redraw();
    void on_actionStart_activated();
    void on_actionQuit_triggered();
    void on_actionClear_triggered();

private:
    QImage myFieldImage();
    QImage enemyFieldImage();
    QImage getFieldImage(char);
private:
    Ui::MainWindow *ui;
    Images *pictures;
    State state;
    Model *model;
    Controller *controller;
};

#endif // MAINWINDOW_H
