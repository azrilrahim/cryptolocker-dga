#ifndef CRYPTOLOCKERDGA_H
#define CRYPTOLOCKERDGA_H

#include <QObject>
#include <QFile>
#include <stdio.h>
#include <QList>
#include <QDebug>

class CryptolockerDga
{
public:
    CryptolockerDga();
    ~CryptolockerDga();
    int executeA(int argc, char *argv[]);

private:
    QList<QString> TLDN;
    void createDomainA(quint16 numOfDomain = 0, quint32 dd = 0, quint32 mm = 0, quint32 yy = 0, QString of = NULL);
    void createDomainB(quint16 numOfDomain = 0, quint32 dd = 0, quint32 mm = 0, quint32 yy = 0, QString of = NULL);

};

#endif // CRYPTOLOCKERDGA_H
