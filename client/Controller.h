#pragma once
#include <QObject>
#include <QPoint>
#include <QTcpSocket>
#include <QRegExp>
#include <QMessageBox>
#include "Model.h"
#include "Utility.h"
#include "ConnectionInfoDialog.h"

enum DisconnectStatus
{
    DS_WIN,
    DS_LOSE,
    DS_WRONG_FIELD,
    DS_WRONG_LOGIN,
    DS_NONE
};

class Controller: public QWidget
{
    Q_OBJECT
public:
    Controller(Model *model_);
    void onMousePressed(const QPoint& position);
    void onGameStart();
    void onGameQuit();
    void clearFields();
    State getState() const;
signals:
    void stateChanged();
private slots:
    void onDataReceived();
    void onConnected();
    void onError( QAbstractSocket::SocketError socketError );
private:
    void parseData(const QString& data);
    bool parseGo(const QString& data);
    bool parseGameResult(const QString& data);
    bool parseFields(const QString& data);
private:
    QTcpSocket *client;
    Model *model;
    QHostAddress serverAddress;
    quint16 serverPort;
    DisconnectStatus disconnectStatus;
};
