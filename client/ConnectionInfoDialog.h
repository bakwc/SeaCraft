#pragma once
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <QDialog>
#include <QHostAddress>
#include <QMessageBox>

class ConnectionInfoDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ConnectionInfoDialog( QWidget* parent = 0 );
    ~ConnectionInfoDialog();

    void setAddressString( const QHostAddress& address, quint16 port );
    QString getAddress();
    quint16 getPort();

public slots:
    void accept();

private:
    QLineEdit* addressTextBox;
    QPushButton* cancelButton;
    QPushButton* applyButton;
    QVBoxLayout* verticalLayout;
    QHBoxLayout* horizontalLayout;

    QString address;
    quint16 port;
};
