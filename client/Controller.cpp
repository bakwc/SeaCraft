#include "Controller.h"

const QString& DEFAULT_CONFIG_FILE = "config.ini";

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


    // TODO: Move to seporate function

    if( !QFile::exists(DEFAULT_CONFIG_FILE) )
        {
            qDebug() << "Config file does not exists!";
            return;
        }

    QFile af(DEFAULT_CONFIG_FILE);
        if( !af.open(QFile::ReadOnly) )
            return;

    QByteArray line;
    QRegExp rx("(\\d+\\.\\d+\\.\\d+\\.\\d+):(\\d+):(\\w+):(.+):");
    while( !af.atEnd() )
    {
        line = af.readLine();
        if(rx.indexIn( line ) == -1) continue;
        serverAddress=rx.cap(1);
        serverPort=rx.cap(2).toInt();
    }
}

Controller::~Controller()
{           // TODO: move to seporate function
    QFile file(DEFAULT_CONFIG_FILE);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << QString("%1:%2:%3:%4:\n")
           .arg(serverAddress.toString())
           .arg(serverPort)
           .arg(model->getLogin())
           .arg(model->getPassword())
           ;
    file.close();
}

void Controller::onMousePressed(const QPoint& pos, bool setShip)
{
    if( model->getState() == ST_PLACING_SHIPS )
    {
        QPoint point = getMyFieldCoord( pos );

        if( point.x() == -1 )
            return;

        qDebug() << "Ship at" << point.x() << point.y();
        model->setMyCell( point.x(), point.y(), setShip ? CL_SHIP : CL_CLEAR );
        emit stateChanged();
        return;
    }

    if( model->getState() == ST_MAKING_STEP )
    {
        QPoint point = getEnemyFieldCoord( pos );
        if( point.x() == -1 )
            return;

        qDebug() << "Going to" << point.x() << point.y();
        model->setEnemyCell( point.x(), point.y(), CL_DOT );

        QString cmd;
        cmd = QString( "step:%1:%2:" ).arg( point.x() ).arg( point.y() );
        qDebug () << cmd;
        client->write( cmd.toLocal8Bit() );
        model->setState( ST_WAITING_STEP );
        emit stateChanged();
        return;
    }
}

void Controller::onDataReceived()
{
    QString data;
    data = client->readAll();
    qDebug() << "Data:" << data;
    parseData( data );
    emit stateChanged();
}

void Controller::parseData(const QString& data)
{
    parseGo(data);
    parseFields(data);
    parseGameResult(data);
    parseErrorInfo(data);
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

bool Controller::parseErrorInfo(const QString& data)
{
    QRegExp rx("wronguser:");
    if (rx.indexIn(data)!=-1)
        {
            model->setState(ST_PLACING_SHIPS);
            connectionError=true;
            emit gameError(GEM_WRONG_USER);
            qDebug() << "Wrong user";
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
            model->setState(ST_PLACING_SHIPS);
            model->clearMyField();
            model->clearEnemyField();
            qDebug() << "We win!";
            emit gameResult( GR_WON );
            return true;
        }

    QRegExp rx2("lose:");
    if (rx2.indexIn(data)!=-1)
        {
            model->setState(ST_PLACING_SHIPS);
            model->clearMyField();
            model->clearEnemyField();
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

void Controller::randomField()
{
    if( model->getState() != ST_PLACING_SHIPS )
        return;

    model->clearMyField();

    for( int i = 1, k = 4; i <= 4; i++, k-- )
        for( int j = 0; j < i; j++ )
            placeShipAtRandom(k);
}

void Controller::placeShipAtRandom( int size )
{
    int p;
    int q;
    bool r;
    bool isOk = true;

    while( isOk )
    {
        p = qrand() % ( 10 - size + 1 );
        q = qrand() % ( 10 - size + 1 );
        r = qrand() % 2;

        for(
            int k = r * p + !r * q - 1;
            k < (r * p + !r * q + size + 1);
            k++
        )
            if(
                model->getMyCell(
                    r * k + !r * p,
                    r * q + !r * k
                ) == CL_SHIP ||
                model->getMyCell(
                    r * k + !r * (p - 1),
                    r * (q - 1) + !r * k
                ) == CL_SHIP ||
                model->getMyCell(
                    r * k + !r * (p + 1),
                    r * (q + 1) + !r * k
                ) == CL_SHIP
            )
                isOk = false;

        isOk =! isOk;
    }

    for(
        int k = r * p + !r * q;
        k < (r * p + !r * q + size);
        k++
    )
        model->setMyCell(
            r * k + !r * p,
            r * q + !r * k,
            CL_SHIP
        );
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
    connectionError=false;

    request=QString("mbclient:1:%1:%2:")
            .arg(model->getLogin())
            .arg(model->getPassword());

    client->write(request.toLocal8Bit());
    if (!client->waitForReadyRead(5000)) return;
    if (connectionError) return;

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
