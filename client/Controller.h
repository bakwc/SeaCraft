#pragma once
#include <QObject>
#include <QPoint>
#include <QTcpSocket>
#include <QRegExp>
#include <QMessageBox>
#include <QFile>
#include <QRegExp>
#include "Model.h"
#include "Utility.h"
#include "ConnectionInfoDialog.h"

enum GameResult
{
    GR_NONE = 0,
    GR_WON = 1,
    GR_LOST = -1
};

enum GameErrorMessage
{
    GEM_UNKNOWN = -1,
    GEM_NONE = 0,
    GEM_ALREADY_CONNECTED,
    GEM_WRONG_FIELD,
    GEM_WRONG_USER
};

class Controller: public QWidget
{
    Q_OBJECT
public:
    Controller(Model *model_);
    ~Controller();
    void onMousePressed(const QPoint& position, bool setShip = true);
    void onGameStart();
    void onGameQuit();
    void clearFields();
    void randomField();
    State getState() const;
    void setConnectionInfo(
        const QString& address,
        quint16 port,
        const QString& login,
        const QString& password
    );
    QString getServerAddress() const;
    quint16 getServerPort() const;
    QString getUserLogin() const;

signals:
    void stateChanged();
    void gameResult( GameResult result );
    void gameError( GameErrorMessage message );

private slots:
    void onDataReceived();
    void onConnected();
    void onError( QAbstractSocket::SocketError socketError );

private:
    void parseData(const QString& data);
    bool parseGo(const QString& data);
    bool parseGameResult(const QString& data);
    bool parseFields(const QString& data);
    void placeShipAtRandom(int size);
    bool parseErrorInfo(const QString& data);
    void readConfig();
    void saveConfig();

private:
    QTcpSocket *client;
    Model *model;
    QHostAddress serverAddress;
    quint16 serverPort;
    bool connectionError;
};
