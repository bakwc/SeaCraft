#include "ui_mainwindow.h"
#include "util/PlaySound.h"
#include "AboutDialog.h"
#include "mainwindow.h"

MainWindow::MainWindow( QWidget* parent ):
    QMainWindow( parent ),
    ui( new Ui::MainWindow )
{
    qsrand( QTime::currentTime().msec() );

    ui->setupUi( this );
    pictures.load();

    ui->labelStatus->setStyleSheet( "QLabel { color : #00157f; }" );
    ui->labelOpponent->setStyleSheet( "QLabel { color : #00157f; }" );
    ui->labelOpponent->clear();
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
    connect(
        controller,
        SIGNAL(gameOpponent(QString)),
        this,
        SLOT(changeGameOpponent(QString))
    );

    this->redraw();
}

MainWindow::~MainWindow()
{
    delete controller;
    delete model;
    delete ui;
}

void MainWindow::setStatus( const QString& status )
{
    ui->labelStatus->setText( tr("Status: ") + status );
}

void MainWindow::changeGameOpponent( const QString& name )
{
    ui->labelOpponent->setText( tr("Opponent: ") + name );
}

void MainWindow::paintEvent( QPaintEvent* event )
{
    Q_UNUSED( event );

    const int deltaY = this->centralWidget()->y();

    QPainter painter( this );
    painter.drawImage(
        0,
        deltaY,
        pictures.get("field")
    );

    painter.drawImage( MYFIELD_X, MYFIELD_Y + deltaY, myFieldImage() );
    painter.drawImage( ENEMYFIELD_X, ENEMYFIELD_Y + deltaY, enemyFieldImage() );

    switch( controller->getState() )
    {
    case ST_PLACING_SHIPS:
        setStatus( tr("placing ships") );
        break;

    case ST_MAKING_STEP:
        setStatus( tr("your step") );
        break;

    case ST_WAITING_STEP:
        setStatus( tr("wait for enemy") );
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
                painter.drawImage( i * cfx, j * cfy, pictures.get("dot") );
                break;

            case CL_HALF:
                painter.drawImage(
                    i * cfx,
                    j * cfy,
                    fld ? pictures.get("half") : pictures.get("redhalf")
                );
                break;

            case CL_SHIP:
                painter.drawImage( i * cfx, j * cfy, pictures.get("full") );
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

void MainWindow::closeEvent( QCloseEvent* event )
{
    controller->onGameQuit();
    event->accept();
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

    connectionDialog->setPassword( controller->getUserPassword() );

    if( connectionDialog->exec() != QDialog::Accepted )
        return;

    controller->setConnectionInfo(
        connectionDialog->getAddress(),
        connectionDialog->getPort(),
        connectionDialog->getLogin(),
        connectionDialog->getPassword()
    );
    controller->onGameStart();
    redraw();
}

void MainWindow::redraw()
{
    if( controller->getState() == ST_PLACING_SHIPS )
        ui->labelOpponent->clear();

    if( controller->getState() == ST_PLACING_SHIPS )
    {
        ui->actionStart->setDisabled(false);
        ui->actionLeave->setDisabled(true);
        ui->menuField->setDisabled(false);
    }
    else
    {
        ui->actionStart->setDisabled(true);
        ui->actionLeave->setDisabled(false);
        ui->menuField->setDisabled(true);
    }

    this->update();
}

void MainWindow::on_actionQuit_triggered()
{
    close();
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

    case GEM_SERVER_ERROR:
        messageString = tr( "Server error!" );
        break;

    case GEM_SERVER_CONNECTION_REFUSED:
    case GEM_SERVER_UNAVAILABLE:
        messageString = tr( "Cannot connect to the server." );
        break;

    case GEM_SERVER_CONNECTION_TIMEOUT:
        messageString = tr( "Server unavailable." );
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

void MainWindow::on_actionLeave_activated()
{
    controller->onGameQuit();
    QMessageBox::information(
        this,
        tr("Game Info"),
        tr("You have disconnected!")
    );

    redraw();
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog dlg( this );
    dlg.exec();
}
