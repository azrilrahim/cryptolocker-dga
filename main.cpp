#include <QCoreApplication>
#include <cryptolockerdga.h>

int main(int argc, char *argv[])
{
    CryptolockerDga *c;
    QCoreApplication a(argc, argv);

    c = new CryptolockerDga();
    c->executeA(argc,argv);
    a.exit(1);
    return 0;


    //return a.exec();
}
