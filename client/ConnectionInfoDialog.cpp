#include "ConnectionInfoDialog.h"

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
