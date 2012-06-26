#include "Controller.h"

Controller::Controller(Model *model_):
    model(model_),
    serverAddress( QHostAddress::LocalHost ),
    serverPort( 1234 )
{
    client = new QTcpSocket(this);
    connect(client,SIGNAL(readyRead()),
            this,SLOT(onDataReceived()));

    connect(
        client, SIGNAL( connected() ),
        this, SLOT( onConnected() )
    );

    connect(
        client, SIGNAL( error(QAbstractSocket::SocketError) ),
        this, SLOT( onError(QAbstractSocket::SocketError) )
    );
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
    parseGameResult(data);
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
                if (rx.cap(2)=="kill")
                    model->setEnemyCell(rx.cap(3).toInt(),
                                        rx.cap(4).toInt(),
                                        CL_SHIP);
                if (rx.cap(2)=="miss")
                    model->setEnemyCell(rx.cap(3).toInt(),
                                        rx.cap(4).toInt(),
                                        CL_DOT);
            }

            if (rx.cap(1)=="1")
            {
                if (rx.cap(2)=="half")
                    model->setMyCell(rx.cap(3).toInt(),
                                        rx.cap(4).toInt(),
                                        CL_HALF);
                if (rx.cap(2)=="kill")
                    model->setMyCell(rx.cap(3).toInt(),
                                        rx.cap(4).toInt(),
                                        CL_SHIP);
                if (rx.cap(2)=="miss")
                    model->setMyCell(rx.cap(3).toInt(),
                                        rx.cap(4).toInt(),
                                        CL_DOT);
            }
            return true;
        }
    return false;
}

bool Controller::parseGameResult(const QString& data)
{
    QRegExp rx("win:");
    if (rx.indexIn(data)!=-1)
        {
            model->setState(ST_MAKING_STEP);
            qDebug() << "We win!";
            emit gameResult( GR_WON );
            return true;
        }

    QRegExp rx2("lose:");
    if (rx2.indexIn(data)!=-1)
        {
            model->setState(ST_MAKING_STEP);
            qDebug() << "We lose!";
            emit gameResult( GR_LOST );
            return true;
        }

    return false;
}

void Controller::onGameStart()
{
    if( !model->checkMyField() )
    {
        emit gameError( GEM_WRONG_FIELD );
        return;
    }

    if( client->state() == QAbstractSocket::ConnectedState )
    {
        emit gameError( GEM_ALREADY_CONNECTED );
        return;
    }

    qDebug(
        "Connected to host %s:%d as %s",
        qPrintable( serverAddress.toString() ),
        serverPort,
        qPrintable( model->getLogin() )
    );

    client->connectToHost( serverAddress, serverPort );
}

void Controller::onGameQuit()
{
    if( client->state() == QAbstractSocket::ConnectedState )
    {
        qDebug() << "Disconnecting from host";
        // TODO: send to server information about quiting from game
        client->disconnectFromHost();
        model->setState(ST_PLACING_SHIPS);
    }
}

void Controller::clearFields()
{
    if( model->getState() != ST_PLACING_SHIPS )
        return;

    model->clearEnemyField();
    model->clearMyField();
}

void Controller::onError( QAbstractSocket::SocketError socketError )
{
    Q_UNUSED( socketError );
    qDebug() << client->errorString();
}

void Controller::onConnected()
{
    QString response;
    QString request;
    request=QString("mbclient:1:%1:%2:")
            .arg(model->getLogin())
            .arg(model->getPassword());

    client->write(request.toLocal8Bit());
    if (!client->waitForReadyRead(5000)) return;
    response=client->readAll();
    qDebug() << response;

    request="field:"+model->getMyField()+":";
    client->write(request.toLocal8Bit());

    qDebug() << request;
    model->setState(ST_WAITING_STEP);
}

State Controller::getState() const
{
    return model->getState();
}

void Controller::setConnectionInfo(
    const QString& address,
    quint16 port,
    const QString& login,
    const QString& password
)
{
    serverAddress = QHostAddress( address );
    serverPort = port;
    model->setLogin( login );
    model->setPassword( password );
}

QString Controller::getServerAddress() const
{
    return serverAddress.toString();
}

quint16 Controller::getServerPort() const
{
    return serverPort;
}

QString Controller::getUserLogin() const
{
    return model->getLogin();
}
