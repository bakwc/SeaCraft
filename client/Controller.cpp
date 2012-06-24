#include "Controller.h"

Controller::Controller(Model *model_):model(model_)
{
    client = new QTcpSocket(this);
    connect(client,SIGNAL(readyRead()),
            this,SLOT(onDataReceived()));
}

void Controller::onMousePressed(const QPoint& pos)
{
    if (model->getState()==ST_PLACING_SHIPS)
    {
        QPoint point=getMyFieldCoord(pos);
        if (point.x()==-1) return;
        qDebug() << "Ship at" << point.x() << point.y();
        model->setMyCell(point.x(),point.y(),CL_SHIP);
        stateChanged();
    }
    else if (model->getState()==ST_MAKING_STEP)
    {
        QPoint point=getEnemyFieldCoord(pos);
        if (point.x()==-1) return;
        qDebug() << "Going to" << point.x() << point.y();
        model->setEnemyCell(point.x(),point.y(),CL_DOT);

        QString cmd;
        cmd = QString("step:%1:%2:").arg(point.x()).arg(point.y());
        qDebug () << cmd;
        client->write(cmd.toLocal8Bit());
        model->setState(ST_WAITING_STEP);
        stateChanged();
    }
}

void Controller::onDataReceived()
{
    QString data;
    data=client->readAll();
    qDebug() << "Data:" << data;
    parseData(data);
    stateChanged();
}

void Controller::parseData(const QString& data)
{
    parseGo(data);
    parseFields(data);
}

bool Controller::parseGo(const QString& data)
{
    QRegExp rx("go:");
    if (rx.indexIn(data)!=-1)
        {
            model->setState(ST_MAKING_STEP);
            qDebug() << "Now making step!";
            return true;
        }
    return false;
}

bool Controller::parseFields(const QString& data)
{
    QRegExp rx("field(\\d):(\\w+):(\\d):(\\d):");
    if (rx.indexIn(data)!=-1)
        {
            qDebug() << "Field:" << rx.cap(1);
            qDebug() << "Status:" << rx.cap(2);
            qDebug() << "X:" << rx.cap(3);
            qDebug() << "Y:" << rx.cap(4);
            if (rx.cap(1)=="2")
            {
                if (rx.cap(2)=="half")
                    model->setEnemyCell(rx.cap(3).toInt(),
                                        rx.cap(4).toInt(),
                                        CL_HALF);
            }
            return true;
        }
    return false;
}

void Controller::onGameStart()
{
    QString response;
    QString request;

    qDebug() << "Starting game";
    client->connectToHost("172.28.0.25",1234);

    client->write("mbclient:1:guest:guest:");
    if (!client->waitForReadyRead(5000)) return;
    response=client->readAll();
    qDebug() << response;

    request="field:"+model->getMyField()+":";
    client->write(request.toLocal8Bit());

    qDebug() << request;
    model->setState(ST_WAITING_STEP);
}
