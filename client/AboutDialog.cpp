#include <QPushButton>
#include <QPainter>
#include <QUrl>
#include <QDesktopServices>
#include "Images.h"
#include "AboutDialog.h"

AboutDialog::AboutDialog( QWidget* parent, Qt::WindowFlags f ):
    QDialog( parent ),
    okButton( new QPushButton(this) ),
    labelURL( new QLabel(this) ),
    labelInfo( new QLabel(this) ),
    verticalLayout( new QVBoxLayout(this) )
{
    const QImage& aboutImage = pictures.get( "about" );
    setFixedSize( aboutImage.width(), aboutImage.height() );

    const int BUTTON_W = 120;
    const int BUTTON_H = 24;

    okButton->setText( tr("Ok") );
    okButton->resize( BUTTON_W, BUTTON_H );
    connect( okButton, SIGNAL(clicked()), this, SLOT(accept()) );

    labelURL->setText(
        "<a href=\"http://seacraft.i-port.su\">http://seacraft.i-port.su</a>"
    );
    labelInfo->setText( tr("Our website:") );

    const QString& labelStyle(
        "QLabel { color : #00157f; font-weight: bold; }"
    );
    labelURL->setStyleSheet( labelStyle );
    labelInfo->setStyleSheet( labelStyle );

    labelURL->setAlignment( Qt::AlignCenter );
    labelInfo->setAlignment( Qt::AlignCenter );

    verticalLayout->addStretch();
    verticalLayout->addWidget( labelInfo );
    verticalLayout->addWidget( labelURL );
    verticalLayout->addSpacing( BUTTON_H );
    verticalLayout->addWidget( okButton );

    connect(
        labelURL,
        SIGNAL(linkActivated(QString)),
        this,
        SLOT(on_labelURL_clicked(QString))
    );
}

void AboutDialog::paintEvent( QPaintEvent* event )
{
    Q_UNUSED( event );
    QPainter painter( this );
    painter.drawImage(
        0,
        0,
        pictures.get("about")
    );
}

void AboutDialog::on_labelURL_clicked( const QString& url )
{
    QDesktopServices::openUrl( QUrl(url) );
}
