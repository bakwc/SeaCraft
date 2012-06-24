#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow( QWidget* parent ):
    QMainWindow( parent ),
    ui( new Ui::MainWindow )
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

void MainWindow::on_actionStart_activated()
{
    QString response;
    QString request;
    qDebug() << "Pressed start";
    client->connectToHost( "172.28.0.25", 1234 );
    client->write( "mbclient:1:guest:guest:" );

    if( !client->waitForReadyRead(5000) )
        return;

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

void MainWindow::onDataReceived()
{
    QString data;
    data = client->read( 500 );
    qDebug() << "Data:" << data;
    parseData( data );
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
