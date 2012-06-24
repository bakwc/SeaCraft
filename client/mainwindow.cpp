#include <QMessageBox>

#include "mainwindow.h"
#include "ui_mainwindow.h"

ConnectionInfoDialog::ConnectionInfoDialog( QWidget* parent ):
    QDialog( parent )
{
    verticalLayout = new QVBoxLayout( this );
    horizontalLayout = new QHBoxLayout( this );
    addressTextBox = new QLineEdit( this );
    applyButton = new QPushButton( this );
    cancelButton = new QPushButton( this );

    verticalLayout->addWidget( addressTextBox );
    verticalLayout->addLayout( horizontalLayout );
    horizontalLayout->addWidget( applyButton );
    horizontalLayout->addWidget( cancelButton );

    this->setFixedSize( 300, 80 );

    applyButton->setText( tr("Apply") );
    cancelButton->setText( tr("Cancel") );

    applyButton->setDefault( true );

    connect( applyButton, SIGNAL(clicked()), this, SLOT(accept()) );
    connect( cancelButton, SIGNAL(clicked()), this, SLOT(reject()) );
}

ConnectionInfoDialog::~ConnectionInfoDialog()
{
}

void ConnectionInfoDialog::setAddressString(
    const QHostAddress& address,
    quint16 port
)
{
    if( !addressTextBox )
        return;

    addressTextBox->setText(
        QString("%1:%2").arg(address.toString()).arg(port)
    );
}

QString ConnectionInfoDialog::getAddress()
{
    return address;
}

quint16 ConnectionInfoDialog::getPort()
{
    return port;
}

void ConnectionInfoDialog::accept()
{
    QHostAddress addr( addressTextBox->text().replace(QRegExp(":.*"), "") );

    bool ok;
    port = addressTextBox->text().replace( QRegExp(".*:"), "" ).toInt( &ok );

    if( !addr.isNull() && ok )
    {
        address = addr.toString();
        qDebug(
            "Server address and port: %s:%d",
            qPrintable( address ),
            port
        );
        done( QDialog::Accepted );
        return;
    }

    QMessageBox::warning(
        this,
        tr( "Warning" ),
        tr( "Specify the valid IPv4 address" )
    );
    /*
    QMessageBox msgbox;
    msgbox.setText( tr("Specify the valid IPv4 address") );
    msgbox.exec();
    */
    addressTextBox->setFocus();
}

MainWindow::MainWindow( QWidget* parent ):
    QMainWindow( parent ),
    ui( new Ui::MainWindow ),
    serverAddress( QHostAddress::LocalHost ),
    serverPort( 1234 )
{
    ui->setupUi( this );
    pictures = new Images;
    pictures->load();
    myField = new Field( pictures, 40, 39, 216, 217 );
    enemyField = new Field( pictures, 322, 39, 214, 217 );
    client = new QTcpSocket( this );
    connect(
        client,
        SIGNAL( readyRead() ),
        this,
        SLOT( onDataReceived() )
    );
    // field1->setCell(0,0,CL_SHIP);
    myField->redraw();
    enemyField->redraw();
    state = ST_PLACING_SHIPS;

    connect(
        client,
        SIGNAL( connected() ),
        this,
        SLOT( onConnected() )
    );
    connect(
        client,
        SIGNAL( error(QAbstractSocket::SocketError) ),
        this,
        SLOT( onError(QAbstractSocket::SocketError) )
    );
    connect( ui->actionQuit, SIGNAL(triggered()), this, SLOT(close()) );
}

MainWindow::~MainWindow()
{
    delete pictures;
    delete ui;
}

void MainWindow::paintEvent( QPaintEvent* event )
{
    Q_UNUSED( event );
    QPainter painter( this );
    painter.drawImage(
        0,
        this->menuBar()->geometry().height(),
        pictures->get( "field" )
    );
    painter.drawImage(
        myField->getX(),
        this->menuBar()->geometry().height() + myField->getY(),
        myField->getImage()
    );
    painter.drawImage(
        enemyField->getX(),
        this->menuBar()->geometry().height() + enemyField->getY(),
        enemyField->getImage()
    );
}

void MainWindow::mousePressEvent( QMouseEvent* ev )
{
    QPoint pos = ev->pos();
    pos.setY( pos.y() - this->menuBar()->geometry().height() );

    if( state == ST_PLACING_SHIPS )
    {
        QPoint point = myField->getCoord( pos.x(), pos.y() );

        if( point.x() == -1 )
            return;

        qDebug() << "Ship at" << point.x() << point.y();
        myField->setCell( point.x(), point.y(), CL_SHIP );
        myField->redraw();
        this->update();
    }
    else
    if( state == ST_MAKING_STEP )
    {
        QPoint point = enemyField->getCoord( pos.x(), pos.y() );

        if( point.x() == -1 )
            return;

        qDebug() << "Going to" << point.x() << point.y();
        enemyField->setCell( point.x(), point.y(), CL_DOT );
        enemyField->redraw();
        this->update();
        //TODO: network code here
        QString cmd;
        cmd = QString( "step:%1:%2:" ).arg( point.x() ).arg( point.y() );
        qDebug() << cmd;
        client->write( cmd.toLocal8Bit() );
        state = ST_WAITING_STEP;
    }
}

void MainWindow::on_actionConnect_triggered()
{
    if( client->state() == QAbstractSocket::ConnectedState )
    {
        QMessageBox::information(
            this,
            tr( "Connection Info" ),
            tr( "Already connected" )
        );
        return;
    }

    ConnectionInfoDialog* connectionDialog = new ConnectionInfoDialog( this );

    connectionDialog->setAddressString( serverAddress, serverPort );
    connectionDialog->setModal( true );
    if( connectionDialog->exec() != QDialog::Accepted )
    {
        qDebug() << "ConnectionDialog::rejected";
        return;
    }

    serverAddress = connectionDialog->getAddress();
    serverPort = connectionDialog->getPort();

    qDebug(
        "Connected to host %s:%d",
        qPrintable( serverAddress.toString() ),
        serverPort
    );

    client->connectToHost( serverAddress, serverPort );
}

void MainWindow::on_actionDisconnect_triggered()
{
    if( client->state() == QAbstractSocket::ConnectedState )
    {
        qDebug() << "Disconnecting from host";
        client->disconnectFromHost();
        state = ST_PLACING_SHIPS;
    }
}

void MainWindow::onError( QAbstractSocket::SocketError socketError )
{
    Q_UNUSED( socketError );
    qDebug() << client->errorString();
    QMessageBox::critical(
        this,
        tr("Connection Error"),
        client->errorString()
    );
}

void MainWindow::onDataReceived()
{
    QString data;
    data = client->read( 500 );
    qDebug() << "Data:" << data;
    parseData( data );
}

void MainWindow::onConnected()
{
    QString response;
    QString request;

    client->write( "mbclient:1:guest:guest:" );

    if( !client->waitForReadyRead(5000) )
    {
        qDebug() << "Wait for ready read timeout";
        return;
    }

    response = client->readAll();
    qDebug() << response;
    request = "field:" + myField->getField() + ":";
    client->write( request.toLocal8Bit() );
    qDebug() << request;
    state = ST_WAITING_STEP;
    //clientStream << ; // TODO: login+pass
    //
    //client->waitForReadyRead();
    //clientStream >> response;
}

void MainWindow::parseData( const QString& data )
{
    parseGo( data );
    parseFields( data );
}

bool MainWindow::parseGo( const QString& data )
{
    QRegExp rx( "go:" );

    if( rx.indexIn( data ) != -1 )
    {
        state = ST_MAKING_STEP;
        qDebug() << "Now making step!";
        return true;
    }

    return false;
}

bool MainWindow::parseFields( const QString& data )
{
    QRegExp rx( "field(\\d):(\\w+):(\\d):(\\d):" );

    if( rx.indexIn( data ) != -1 )
    {
        qDebug() << "Field:" << rx.cap( 1 );
        qDebug() << "Status:" << rx.cap( 2 );
        qDebug() << "X:" << rx.cap( 3 );
        qDebug() << "Y:" << rx.cap( 4 );

        if( rx.cap(1) == "2" )
        {
            if( rx.cap(2) == "half" )
            {
                enemyField->setCell(
                    rx.cap( 3 ).toInt(),
                    rx.cap( 4 ).toInt(),
                    CL_HALF
                );
            }
        }

        myField->redraw();
        enemyField->redraw();
        this->update();
        return true;
    }

    return false;
}
