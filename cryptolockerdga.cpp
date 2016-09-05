#include "cryptolockerdga.h"

CryptolockerDga::CryptolockerDga()
{
    this->TLDN.append("com");
    this->TLDN.append("net");
    this->TLDN.append("biz");
    this->TLDN.append("ru");
    this->TLDN.append("org");
    this->TLDN.append("co.uk");
    this->TLDN.append("info");
}

CryptolockerDga::~CryptolockerDga()
{

}

int CryptolockerDga::executeA(int argc, char *argv[])
{
    quint64 numOfDomain;
    QString outfile;
    QString agStr;
    quint32 DD,MM,YY;
    QString dateAttack;

    numOfDomain = 0;
    outfile.clear();

    DD = 0;
    MM = 0;
    YY = 0;

    for (int i = 1; i < argc; i++){
        agStr = agStr + argv[i] + " ";
    }

    agStr = agStr.replace("&", " ");
    //printf("final agStr: %s\n",agStr.toStdString().c_str());
    QStringList agStrL = agStr.trimmed().split(" ");

    //qDebug() << "ag size:" << agStrL.size();

    for (int ag = 0; ag < agStrL.size(); ag++)
    {
        //qDebug() << "ag:" << ag;
        agStr = agStrL[ag];
        agStr = agStr.trimmed();

        if (agStr.isEmpty()){
            continue;
        }



        //printf("%s\n",agStr.toStdString().c_str());
        QStringList argvL = agStr.split("=");

        if (argvL.size() != 2){
            printf("Invalid argument for input #%d\n",ag);
            return 0;
        }

        if (argvL.at(0) == "n"){
            numOfDomain = argvL.at(1).toInt();
            continue;
        }


        if (argvL.at(0) == "d")
        {
            dateAttack = argvL.at(1);
            QList<QString>dt = dateAttack.trimmed().split("-");

            DD = dt.at(0).toInt();
            MM = dt.at(1).toInt();
            YY = dt.at(2).toInt();
            //qDebug() << "done date";
            continue;
        }

        if (argvL.at(0) == "f"){

            outfile = argvL.at(1);
            continue;
        }

    }

    //qDebug() << "of looping";

    if (numOfDomain == 0)
    {
        printf("CrytoLocker: Missing or Invalid number of domain to be generated\n");
        return -1;
    }

    if (dateAttack.trimmed().isEmpty())
    {
        printf("Missing or Invalid date\n");
        return -1;
    }

    this->createDomainA(numOfDomain,DD,MM,YY,outfile);
    this->createDomainB(numOfDomain,DD,MM,YY,outfile);
    return 0;
}

void CryptolockerDga::createDomainA(quint16 numOfDomain, quint32 dd, quint32 mm, quint32 yy, QString of)
{
    //based on https://github.com/pchaigno/dga-collection/blob/master/Cryptolocker.py

    quint32 len;
    QString domain;
    quint32 key;
    QFile fs;


    if ((numOfDomain == 0) || (dd == 0) || (mm == 0) || (yy == 0)){

        printf("cryptolocker dga: invalid input\n");
        return;
    }

    if (!of.trimmed().isEmpty()){

        fs.setFileName(of);
        fs.open(QIODevice::WriteOnly | QIODevice::Text);
    }

    for (key = 0; key < numOfDomain; key++){

        yy = yy + key;
        dd *=65537;
        mm *=65537;
        yy *=65537;

        len = (dd >> 3) ^ (yy >> 8) ^ (yy >> 11);
        len &= 3;
        len +=12;

        domain.clear();

        for (quint32 n = 1; n <= len; n++){

            dd = ((dd << 13 & 0xFFFFFFFF) >> 19 & 0xFFFFFFFF) ^ ((dd >> 1 & 0xFFFFFFFF) << 13 & 0xFFFFFFFF) ^ (dd >> 19 & 0xFFFFFFFF);
            dd &= 0xFFFFFFFF;


            mm = ((mm << 2 & 0xFFFFFFFF) >> 25 & 0xFFFFFFFF) ^ ((mm >> 3 & 0xFFFFFFFF) << 7 & 0xFFFFFFFF)  ^ (mm >> 25 & 0xFFFFFFFF);
            mm &= 0xFFFFFFFF;
            yy = ((yy << 3 & 0xFFFFFFFF) >> 11 & 0xFFFFFFFF) ^ ((yy >> 4 & 0xFFFFFFFF) << 21 & 0xFFFFFFFF) ^ (yy >> 11 & 0xFFFFFFFF);
            yy &= 0xFFFFFFFF;

            domain.append(QChar(97 + (yy ^ mm ^ dd) %25));
        }

        domain.append(".");
        domain.append(this->TLDN.at(key % 7).toLocal8Bit());
        if (!of.isEmpty()){
            domain.append('\n');
            fs.write(domain.toLocal8Bit());
        }
        else
            printf("%s\n",domain.toStdString().c_str());
    }

    if (!of.isEmpty())
        fs.close();
}

void CryptolockerDga::createDomainB(quint16 numOfDomain, quint32 dd, quint32 mm, quint32 yy, QString of)
{
    //https://blog.fortinet.com/post/a-closer-look-at-cryptolocker-s-dga

    quint32 len;
    QString domain;
    quint32 key;
    QFile fs;


    if ((numOfDomain == 0) || (dd == 0) || (mm == 0) || (yy == 0)){

        printf("cryptolocker dga: invalid input\n");
        return;
    }

    if (!of.trimmed().isEmpty()){

        fs.setFileName(of);
        fs.open(QIODevice::WriteOnly | QIODevice::Text);
    }

    for (key = 0; key < numOfDomain; key++){

        quint32 dk = (dd << 0x10) ^ dd;
        if (dk <=1)
            dk = dd << 0x18;

        quint32 mk = (mm << 0x10) ^ mm;
        if (mk <=7){
            mk = mm << 0x18;
            if (mk <=7)
                mk = !(mk);
        }

        quint32 yk = ((yy + key) << 0x10) ^ (yy + key);
        if (yk <= 0xf)
            yk = (yy + key) << 0x18;

        len = (((dk ^ ((yk ^ 8 * yk ^ ((dk ^ ((mk ^ 4 * mk) >> 6)) >> 8)) >> 5)) >> 6) & 3) + 0xc;

        domain.clear();

        for (quint32 n = 1; n <= len; n++){

            mk = ((mk ^ 4 * mk) >> 0x19) ^ 0x10 * (mk & 0xFFFFFFF8);
            dk = (((dk >> 0x13) ^ ((dk >> 6) ^ (dk << 0xc))) & 0x1FFF) ^ (dk << 0xC);
            yk = ((yk ^ 8 * yk) >> 0x8) ^ ((yk & 0xFFFFFFF0) << 0x11);


            domain.append(QChar((dk ^ mk ^ yk) % 0x19 + 97));
        }

        domain.append(".");
        domain.append(this->TLDN.at(key % 7).toLocal8Bit());
        if (!of.isEmpty()){
            domain.append('\n');
            fs.write(domain.toLocal8Bit());
        }
        else
            printf("%s\n",domain.toStdString().c_str());
    }

    if (!of.isEmpty())
        fs.close();
}
