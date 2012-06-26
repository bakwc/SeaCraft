#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    pictures = new Images;
    pictures->load();

    ui->label->setStyleSheet(
                "QLabel { color : #00157f; }");
    model = new Model;
    controller = new Controller(model);
    connect(controller,SIGNAL(stateChanged()),this,SLOT(redraw()));
}

MainWindow::~MainWindow()
{
    delete controller;
    delete model;
    delete pictures;
    delete ui;
}

void MainWindow::setStatus(const QString& status)
{
    ui->label->setText("Status: "+status);
}

void MainWindow::paintEvent( QPaintEvent *event )
{
    QPainter painter(this);
    painter.drawImage(0,this->menuBar()->geometry().height(),
                      pictures->get("field"));

    painter.drawImage(MYFIELD_X, MYFIELD_Y, myFieldImage());
    painter.drawImage(ENEMYFIELD_X, ENEMYFIELD_Y, enemyFieldImage());
    switch (controller->getState())
    {
    case ST_PLACING_SHIPS:
        setStatus("placing ships");
        break;
    case ST_MAKING_STEP:
        setStatus("your step");
        break;
    case ST_WAITING_STEP:
        setStatus("wait for enemy");
        break;
    }
}

QImage MainWindow::myFieldImage()
{
    return getFieldImage(0);
}

QImage MainWindow::enemyFieldImage()
{
    return getFieldImage(1);
}

QImage MainWindow::getFieldImage(char fld)
{
    QImage image(FIELD_WIDTH,FIELD_HEIGHT,QImage::Format_ARGB32);
    Cell cell;
    image.fill(0);
    QPainter painter(&image);

    double cfx=1.0*FIELD_WIDTH/10.0;
    double cfy=1.0*FIELD_HEIGHT/10.0;
    for (int i=0;i<10;i++)
        for (int j=0;j<10;j++)
        {
            if (fld==0) cell=model->getMyCell(i,j);
            else cell=model->getEnemyCell(i,j);
            switch(cell)
            {
                case CL_DOT:
                painter.drawImage(i*cfx,j*cfy,pictures->get("dot"));
                break;
                case CL_HALF:
                painter.drawImage(i*cfx,j*cfy,pictures->get("half"));
                break;
                case CL_SHIP:
                painter.drawImage(i*cfx,j*cfy,pictures->get("full"));
                break;
            default: break;
            }
        }
    return image;
}


void MainWindow::mousePressEvent(QMouseEvent * ev)
{
    QPoint pos=ev->pos();
    controller->onMousePressed(pos);
}

void MainWindow::on_actionStart_activated()
{
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
