#include "Controller.h"

const QString& DEFAULT_CONFIG_FILE = "config.ini";
const quint16 DEFAULT_SERVER_PORT = 1234;
const quint16 DEFAULT_SERVER_TIMEOUT = 5000;

Controller::Controller( Model* model_ ):
    model( model_ ),
    serverAddress( QHostAddress::LocalHost ),
    serverPort( DEFAULT_SERVER_PORT )
{
    client = new QTcpSocket( this );
    connect(
        client, SIGNAL(readyRead()),
        this, SLOT(onDataReceived())
    );

    connect(
        client, SIGNAL(connected()),
        this, SLOT(onConnected())
    );

    connect(
        client, SIGNAL(error(QAbstractSocket::SocketError)),
        this, SLOT(onError(QAbstractSocket::SocketError))
    );

    readConfig();
}

Controller::~Controller()
{
    saveConfig();
}

void Controller::readConfig()
{
    if( !QFile::exists(DEFAULT_CONFIG_FILE) )
    {
        qDebug() << "Config file does not exists!";
        return;
    }

    QFile cf( DEFAULT_CONFIG_FILE );

    if( !cf.open(QFile::ReadOnly) )
        return;

    QByteArray line;
    QRegExp rx( "(\\d+\\.\\d+\\.\\d+\\.\\d+):(\\d+):(\\w+):(.+):" );

    while( !cf.atEnd() )
    {
        line = cf.readLine();

        if( rx.indexIn(line) == -1 )
            continue;

        qDebug() << "Setting connection info";
        setConnectionInfo(
            rx.cap(1),
            rx.cap(2).toUInt(),
            rx.cap(3),
            rx.cap(4)
        );
        break;
    }

    cf.close();
}

void Controller::saveConfig()
{
    QFile cf( DEFAULT_CONFIG_FILE );

    if( !cf.open(QIODevice::WriteOnly | QIODevice::Text) )
        return;

    cf.write(
        qPrintable(
            QString("%1:%2:%3:%4:\n")
            .arg(serverAddress.toString())
            .arg(serverPort)
            .arg(model->getLogin())
            .arg(model->getPassword())
        )
    );
    cf.close();
}

void Controller::onMousePressed( const QPoint& pos, bool setShip )
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
        if( model->getEnemyCell(point.x(), point.y()) == CL_CLEAR )
            model->setEnemyCell( point.x(), point.y(), CL_DOT );

        QString cmd;
        cmd = QString( "step:%1:%2:" ).arg( point.x() ).arg( point.y() );
        qDebug() << cmd;
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

void Controller::parseData( const QString& data )
{
    parseFound( data );
    parseWrongField( data );
    parseWrongStep( data );
    parseFields( data );
    parseGo( data );
    parseGameResult( data );
    parseErrorInfo( data );
}

bool Controller::parseFound( const QString& data )
{
    QRegExp rx( "found:((\\w|\\d)+):" );

    if( rx.indexIn(data) == -1 )
        return false;

    qDebug() << "Found opponent:" << rx.cap( 1 );
    emit gameOpponent( rx.cap(1) );
    return true;
}

bool Controller::parseGo( const QString& data )
{
    QRegExp rx( "go:" );

    if( rx.indexIn(data) == -1 )
        return false;

    qDebug() << "Now making step!";
    model->setState( ST_MAKING_STEP );
    return true;
}

bool Controller::parseErrorInfo( const QString& data )
{
    QRegExp rx( "wronguser:" );

    if( rx.indexIn(data) == -1 )
        return false;

    qDebug() << "Wrong user";
    model->setState( ST_PLACING_SHIPS );
    connectionError = true;
    emit gameError( GEM_WRONG_USER );
    return true;
}

bool Controller::parseWrongStep( const QString& data )
{
    QRegExp rx( "wrongstep:" );

    if( rx.indexIn(data) == -1 )
        return false;

    qDebug() << "Maked wrong step";
    model->setState( ST_MAKING_STEP );
    return true;
}

bool Controller::parseWrongField( const QString& data )
{
    QRegExp rx( "wrongfield:" );

    if( rx.indexIn(data) == -1 )
        return false;

    qDebug() << "Maked wrong field";
    model->setState( ST_PLACING_SHIPS );
    return true;
}

bool Controller::parseFields( const QString& data )
{
    QRegExp rx( "field(\\d):(\\w+):(\\d):(\\d):" );

    int pos = 0;
    while( (pos = rx.indexIn(data, pos)) != -1 )
    {
        const QString& type = rx.cap( 2 );
        int field = rx.cap( 1 ).toInt();
        int xpos = rx.cap( 3 ).toInt();
        int ypos = rx.cap( 4 ).toInt();

        Cell cell = type == "half"
            ? CL_HALF
            : type == "kill"
            ? CL_HALF
            : CL_DOT;

        if( field == 2 )
            model->setEnemyCell( xpos, ypos, cell );
        else
            model->setMyCell( xpos, ypos, cell );

        pos += rx.matchedLength();
    }

    return pos;
}

bool Controller::parseGameResult( const QString& data )
{
    QRegExp rx( "win:" );

    if( rx.indexIn(data) != -1 )
    {
        qDebug() << "We win!";
        emit gameResult( GR_WON );
        model->setState( ST_PLACING_SHIPS );
        model->clearMyField();
        model->clearEnemyField();
        return true;
    }

    QRegExp rx2( "lose:" );

    if( rx2.indexIn(data) != -1 )
    {
        qDebug() << "We lose!";
        emit gameResult( GR_LOST );
        model->setState( ST_PLACING_SHIPS );
        model->clearMyField();
        model->clearEnemyField();
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
        qPrintable(serverAddress.toString()),
        serverPort,
        qPrintable(model->getLogin())
    );

    client->connectToHost( serverAddress, serverPort );
}

void Controller::onGameQuit()
{
    if( client->state() == QAbstractSocket::ConnectedState )
    {
        qDebug() << "Disconnecting from host";
        client->write( "disconnect:" );
        client->disconnectFromHost();
        model->setState( ST_PLACING_SHIPS );
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
            placeShipAtRandom( k );
}

void Controller::placeShipAtRandom( int size )
{
    int p;
    int q;
    bool r;
    bool isOk = true;

    while( isOk )
    {
        r = qrand() % 2;
        p = qrand() % ( 10 - r * (size + 1) );
        q = qrand() % ( 10 - !r * (size + 1) );

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

        isOk = ! isOk;
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
    connectionError = false;

    request = QString( "mbclient:2:%1:%2:" )
        .arg( model->getLogin() )
        .arg( model->getPassword() );

    client->write( request.toLocal8Bit() );

    if( !client->waitForReadyRead(DEFAULT_SERVER_TIMEOUT) )
        return;

    if( connectionError )
        return;

    response = client->readAll();
    qDebug() << response;

    request = "field:4:" + model->getMyField() + ":";
    client->write( request.toLocal8Bit() );

    qDebug() << request;
    model->setState( ST_WAITING_STEP );
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
