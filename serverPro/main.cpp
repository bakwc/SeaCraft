#include "Application.h"

int main( int argc, char *argv[] )
{
/*
    FieldPro f;
    f.initField(
        "00001000200200000040030002003000000400001000030000"
        "00100000200000000030577600000000000056000000100000"
    );
    f.showField();

    qDebug() << QString( f.checkField() ? "valid field" : "not valid field" );

    return 0;
*/
    Application app( argc, argv );

    app.parceCmdLine();
    if( !app.spawnServer() )
        return 1;

    return app.exec();
}
