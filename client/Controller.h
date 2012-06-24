#pragma once
#include <QObject>
#include <QPoint>
#include <QTcpSocket>
#include <QRegExp>
#include <QMessageBox>
#include "Model.h"
#include "Utility.h"
#include "ConnectionInfoDialog.h"

class Controller: public QWidget
{
    Q_OBJECT
public:
    Controller(Model *model_);
    void onMousePressed(const QPoint& position);
    void onGameStart();
signals:
    void stateChanged();
private slots:
    void onDataReceived();
    void onConnected();
    void onError( QAbstractSocket::SocketError socketError );
    void on_actionDisconnect_triggered();
private:
    void parseData(const QString& data);
    bool parseGo(const QString& data);
    bool parseFields(const QString& data);
private:
    QTcpSocket *client;
    Model *model;
    QHostAddress serverAddress;
    quint16 serverPort;
};
