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
#include "Field.h"


namespace Ui
{
class MainWindow;
}

enum State
{
    ST_PLACING_SHIPS,
    ST_WAITING_STEP,
    ST_MAKING_STEP
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow( QWidget* parent = 0 );
    ~MainWindow();

protected:
    void paintEvent( QPaintEvent* event );
    void mousePressEvent( QMouseEvent* ev );

private slots:
    void on_actionStart_activated();
    void onDataReceived();

private:
    void parseData( const QString& data );
    bool parseGo( const QString& data );
    bool parseFields( const QString& data );

private:
    Ui::MainWindow* ui;
    Images* pictures;
    Field* myField;
    Field* enemyField;
    State state;
    QTcpSocket* client;
};

#endif // MAINWINDOW_H
