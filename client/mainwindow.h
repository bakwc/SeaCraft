#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QPainter>
#include <QPoint>
#include <QMouseEvent>
#include <QDebug>
#include <QtNetwork/QTcpSocket>
#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLayout>
#include <QHostAddress>

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

class ConnectionInfoDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ConnectionInfoDialog( QWidget* parent = 0 );
    ~ConnectionInfoDialog();

    void setAddressString( const QHostAddress& address, quint16 port );
    QString getAddress();
    quint16 getPort();

public slots:
    void accept();

private:
    QLineEdit* addressTextBox;
    QPushButton* cancelButton;
    QPushButton* applyButton;
    QVBoxLayout* verticalLayout;
    QHBoxLayout* horizontalLayout;

    QString address;
    quint16 port;
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
    void onDataReceived();
    void onConnected();
    void onError( QAbstractSocket::SocketError socketError );

    void on_actionConnect_triggered();

    void on_actionDisconnect_triggered();

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
    QHostAddress serverAddress;
    quint16 serverPort;
};

#endif // MAINWINDOW_H
