#pragma once
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <QDialog>
#include <QHostAddress>
#include <QMessageBox>
#include <QLabel>

class ConnectionInfoDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ConnectionInfoDialog( QWidget* parent = 0 );
    ~ConnectionInfoDialog();

    void setAddressString( const QHostAddress& address, quint16 port );
    QString getAddress() const;
    QString getLogin() const;
    QString getPassword() const;
    quint16 getPort();

public slots:
    void accept();

private:
    QLineEdit* addressTextBox;
    QLineEdit* loginTextBox;
    QLineEdit* passTextBox;

    QLabel* addrLabel;
    QLabel* loginLabel;
    QLabel* passLabel;

    QPushButton* cancelButton;
    QPushButton* applyButton;

    QHBoxLayout* addrLayout;
    QHBoxLayout* loginLayout;
    QHBoxLayout* passLayout;
    QHBoxLayout* buttonLayout;
    QVBoxLayout* verticalLayout;

    QGridLayout* layout;

    QString address;
    QString login;
    QString pass;
    quint16 port;
};
