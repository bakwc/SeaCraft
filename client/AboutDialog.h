#pragma once

#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog( QWidget* parent = 0, Qt::WindowFlags f = 0 );

public slots:
    void on_labelURL_clicked( const QString& url );

protected:
    void paintEvent( QPaintEvent* event );
    QPushButton* okButton;
    QLabel* labelURL;
    QLabel* labelInfo;
    QVBoxLayout* verticalLayout;
};
