#include <QApplication>
#include <QtPlugin>
#include <QTranslator>
#include "mainwindow.h"

int main( int argc, char* argv[] )
{
    QApplication a( argc, argv );
    QTranslator translator;

    QString lang = QLocale::languageToString(
        QLocale::system().language()
    ).toLower();
    qDebug( "Locale: %s", qPrintable(lang) );
    translator.load( ":/" + lang );
    a.installTranslator( &translator );

    MainWindow w;
    w.show();
    return a.exec();
}
