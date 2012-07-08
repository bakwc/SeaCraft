#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QPainter>
#include <QPoint>
#include <QMouseEvent>
#include <QDebug>
#include <QtNetwork/QTcpSocket>
#include <QString>
#include <QTime>
#include <QVBoxLayout>
#include "Images.h"
#include "Model.h"
#include "Controller.h"
#include "Defines.h"
#include "util/PlaySound.h"

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow( QWidget* parent = 0 );
    ~MainWindow();

protected:
    void paintEvent( QPaintEvent* event );
    void mousePressEvent( QMouseEvent* ev );
    void closeEvent( QCloseEvent* event );

private slots:
    void redraw();
    void showGameResult( GameResult result );
    void showGameError( GameErrorMessage message );
    void changeGameOpponent( const QString& name );
    void on_actionStart_activated();
    void on_actionQuit_triggered();
    void on_actionClear_triggered();
    void on_actionRandom_triggered();
    void on_actionLeave_activated();
    void on_actionAbout_triggered();

private:
    void setStatus( const QString& status );

private:
    QImage myFieldImage();
    QImage enemyFieldImage();
    QImage getFieldImage( char );

private:
    Ui::MainWindow* ui;
    State state;
    Model* model;
    Controller* controller;
};

#endif // MAINWINDOW_H
