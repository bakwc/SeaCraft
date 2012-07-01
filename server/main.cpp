#include "Application.h"

int main( int argc, char *argv[] )
{
    Application app( argc, argv );

    if( !app.init() )
        return 1;

    return app.exec();
}
