#pragma once
#include <QObject>
#include <QPoint>
#include <QTcpSocket>
#include <QRegExp>
#include "Model.h"
#include "Utility.h"

class Controller: public QObject
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
private:
    void parseData(const QString& data);
    bool parseGo(const QString& data);
    bool parseFields(const QString& data);
private:
    QTcpSocket *client;
    Model *model;
};
