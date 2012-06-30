#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow( QWidget* parent ):
    QMainWindow( parent ),
    ui( new Ui::MainWindow )
{
    qsrand( QTime::currentTime().msec() );

    ui->setupUi( this );
    pictures = new Images;
    pictures->load();

    ui->label->setStyleSheet( "QLabel { color : #00157f; }" );
    model = new Model;
    controller = new Controller( model );

    connect( controller, SIGNAL(stateChanged()), this, SLOT(redraw()) );
    connect(
        controller,
        SIGNAL(gameResult(GameResult)),
        this,
        SLOT(showGameResult(GameResult))
    );
    connect(
        controller,
        SIGNAL(gameError(GameErrorMessage)),
        this,
        SLOT(showGameError(GameErrorMessage))
    );
}

MainWindow::~MainWindow()
{
    delete controller;
    delete model;
    delete pictures;
    delete ui;
}

void MainWindow::setStatus( const QString& status )
{
    ui->label->setText( "Status: " + status );
}

void MainWindow::paintEvent( QPaintEvent* event )
{
    Q_UNUSED( event );

    const int deltaY = this->centralWidget()->y();

    QPainter painter( this );
    painter.drawImage(
        0,
        deltaY,
        pictures->get("field")
    );

    painter.drawImage( MYFIELD_X, MYFIELD_Y + deltaY, myFieldImage() );
    painter.drawImage( ENEMYFIELD_X, ENEMYFIELD_Y + deltaY, enemyFieldImage() );

    switch( controller->getState() )
    {
    case ST_PLACING_SHIPS:
        setStatus( "placing ships" );
        break;

    case ST_MAKING_STEP:
        setStatus( "your step" );
        break;

    case ST_WAITING_STEP:
        setStatus( "wait for enemy" );
        break;
    }
}

QImage MainWindow::myFieldImage()
{
    return getFieldImage( 0 );
}

QImage MainWindow::enemyFieldImage()
{
    return getFieldImage( 1 );
}

QImage MainWindow::getFieldImage( char fld )
{
    QImage image( FIELD_WIDTH, FIELD_HEIGHT, QImage::Format_ARGB32 );
    Cell cell;
    image.fill( 0 );
    QPainter painter( &image );

    double cfx = 1.0 * FIELD_WIDTH / 10.0;
    double cfy = 1.0 * FIELD_HEIGHT / 10.0;

    for( int i = 0; i < 10; i++ )
        for( int j = 0; j < 10; j++ )
        {
            if( fld == 0 )
                cell = model->getMyCell( i, j );
            else
                cell = model->getEnemyCell( i, j );

            switch( cell )
            {
            case CL_DOT:
                painter.drawImage( i * cfx, j * cfy, pictures->get("dot") );
                break;

            case CL_HALF:
                painter.drawImage(
                    i * cfx,
                    j * cfy,
                    fld ? pictures->get("half") : pictures->get("redhalf")
                );
                break;

            case CL_SHIP:
                painter.drawImage( i * cfx, j * cfy, pictures->get("full") );
                break;

            default:
                break;
            }
        }

    return image;
}

void MainWindow::mousePressEvent( QMouseEvent* ev )
{
    QPoint pos = ev->pos();
    pos.setY( pos.y() - this->centralWidget()->y() );
    controller->onMousePressed( pos, ev->button() == Qt::LeftButton );
}

void MainWindow::on_actionStart_activated()
{
    ConnectionInfoDialog* connectionDialog = new ConnectionInfoDialog( this );

    connectionDialog->setModal( true );
    connectionDialog->setAddressString(
        controller->getServerAddress(),
        controller->getServerPort()
    );
    connectionDialog->setLogin( controller->getUserLogin() );

    if( connectionDialog->exec() != QDialog::Accepted )
    {
        qDebug() << "ConnectionDialog::rejected";
        return;
    }

    controller->setConnectionInfo(
        connectionDialog->getAddress(),
        connectionDialog->getPort(),
        connectionDialog->getLogin(),
        connectionDialog->getPassword()
    );
    controller->onGameStart();
}

void MainWindow::redraw()
{
    this->update();
}

void MainWindow::on_actionQuit_triggered()
{
    controller->onGameQuit();
    this->close();
}

void MainWindow::on_actionClear_triggered()
{
    controller->clearFields();
    this->update();
}

void MainWindow::showGameResult( GameResult result )
{
    if( result == GR_NONE )
        return;

    QString messageString = result == GR_WON
        ? tr( "You win!" )
        : tr( "You lose!" );

    this->update();
    QMessageBox::information( this, tr("Game result"), messageString );
}

void MainWindow::showGameError( GameErrorMessage message )
{
    QString messageString;

    switch( message )
    {
    case GEM_WRONG_FIELD:
        messageString = tr( "Wrong ships placement!" );
        break;

    case GEM_WRONG_USER:
        messageString = tr( "Wrong user passed!" );
        break;

    case GEM_ALREADY_CONNECTED:
        messageString = tr( "You are already connected!" );
        break;

    default:
        return;
    }

    this->update();
    QMessageBox::information( this, tr("Game Info"), messageString );
}

void MainWindow::on_actionRandom_triggered()
{
    controller->randomField();
    this->update();
}
