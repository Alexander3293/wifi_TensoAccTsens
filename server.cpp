#include "server.h"
#include <QDebug>
#include <QCoreApplication>

server::server(QObject *parent) : QObject(parent)
{
    _sok_udp = new QUdpSocket(this);

    for (int numberDev=0; numberDev < 6; numberDev++){
        listPointDevices.append(new (pointsDevices));
        listPointDevices.at(numberDev)->numDevices = numberDev;
    }
      myHeader[0] = 0xff;
      myHeader[1] = 0x00;
      myHeader[2] = 0xed;

      /*create file, to write data from ESP */
//      logFileName  = QString("D:/test_server/Log_%1__%2.txt")
//              .arg(QDate::currentDate().toString("yyyy_MM_dd"))
//              .arg(QTime::currentTime().toString("hh_mm_ss_zzz"));

//      if(file_global.isOpen())
//          file_global.close();

//      file_global.setFileName(logFileName);
//      if (!file_global.open(QIODevice::Append |QIODevice::WriteOnly))
//      {
//          qDebug() << "Ошибка при открытии файла";
//          return;
//      }


}

server::~server()
{
    delete _sok_udp;
    file_global.flush();
    if(file_global.isOpen())
        file_global.close();
}

void server::initUDP()
{
    _sok_udp = new QUdpSocket();
    host_ = QHostAddress("192.168.1.80");
    port_ = 7070;   //dst port
    //_sok_udp->bind(host_, 7000);
    _sok_udp->bind(host_, 8080);
//    this->port_ = 1500;
    connect(_sok_udp, &QUdpSocket::readyRead, this, &server::slotDataRead);
}

void server::slotDataRead()
{
    QByteArray datagram;
    bool ok;
    int16_t tmp1=0;
    int16_t tmp2=0;
    int16_t value = 0;

//    while(_sok_udp->bytesAvailable() >= OneDatagramSize){
//        datagram = _sok_udp->read(OneDatagramSize);

    while(_sok_udp->hasPendingDatagrams())
    {

        datagram.resize(_sok_udp->pendingDatagramSize());
        QNetworkDatagram datagram_d = _sok_udp->receiveDatagram(-1);

        datagram = datagram_d.data();
        dataDatagramlen =  datagram.length()*2;
        counterDatagram = 0;
        deviceId = -1;

        QString str_data = datagram.toHex();

        if(file_global.isOpen()){
            file_global.write(str_data.toUtf8());       //Write to the File
            file_global.flush();
        }

        QString tmp_str;

        tmp_str = str_data.mid(counterDatagram, 8);
        if(tmp_str.startsWith(espHeader, Qt::CaseSensitivity::CaseInsensitive)) {
            deviceId = tmp_str.rightRef(2).toInt() -1;
            counterDatagram += 8;
        }
        else return;
        QString tmpVal;

        while(counterDatagram < dataDatagramlen){
            tmpVal = str_data.mid(counterDatagram, 4);
            tmp1 = tmpVal.left(2).toInt(&ok,16);
            tmp2 = tmpVal.right(2).toInt(&ok,16);
            value = (int16_t)(tmp2 << 8 | tmp1);
            counterDatagram += 4;
            listPointDevices.at(deviceId)->ADC1.append(value);

            tmpVal = str_data.mid(counterDatagram, 4);
            tmp1 = tmpVal.left(2).toInt(&ok,16);
            tmp2 = tmpVal.right(2).toInt(&ok,16);
            value = (int16_t)(tmp2 << 8 | tmp1);
            counterDatagram += 4;
            listPointDevices.at(deviceId)->ADC2.append(value);

            tmpVal = str_data.mid(counterDatagram, 4);
            tmp1 = tmpVal.left(2).toInt(&ok,16);
            tmp2 = tmpVal.right(2).toInt(&ok,16);
            value = (int16_t)(tmp2 << 8 | tmp1);
            counterDatagram += 4;
            listPointDevices.at(deviceId)->accX.append(value);

            tmpVal = str_data.mid(counterDatagram, 4);
            tmp1 = tmpVal.left(2).toInt(&ok,16);
            tmp2 = tmpVal.right(2).toInt(&ok,16);
            value = (int16_t)(tmp2 << 8 | tmp1);
            counterDatagram += 4;
            listPointDevices.at(deviceId)->accY.append(value);

            tmpVal = str_data.mid(counterDatagram, 4);
            tmp1 = tmpVal.left(2).toInt(&ok,16);
            tmp2 = tmpVal.right(2).toInt(&ok,16);
            value = (int16_t)(tmp2 << 8 | tmp1);
            counterDatagram += 4;
            listPointDevices.at(deviceId)->accZ.append(value);

            tmpVal = str_data.mid(counterDatagram, 4);
            tmp1 = tmpVal.left(2).toInt(&ok,16);
            tmp2 = tmpVal.right(2).toInt(&ok,16);
            value = (int16_t)(tmp2 << 8 | tmp1);
            counterDatagram += 4;
            listPointDevices.at(deviceId)->Tsens.append(value);
        }

        emit newData(listPointDevices.at(deviceId));    /*Передать измерения */

        listPointDevices.at(deviceId)->ADC1.clear();
        listPointDevices.at(deviceId)->ADC2.clear();
        listPointDevices.at(deviceId)->accX.clear();
        listPointDevices.at(deviceId)->accY.clear();
        listPointDevices.at(deviceId)->accZ.clear();
        listPointDevices.at(deviceId)->Tsens.clear();

    }
}

/* Header 3 bytes, 1 byte cmd. 1 setG, 1 setScale = 6 bytes */
void server::cmdEspAllSlot(cmdESP cmd, uint setG, uint setHR)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    if(cmd == START_ESP){
        /*create file, to write data from ESP */
        logFileName  = QString("D:/test_server/Log_%1__%2.txt")
                .arg(QDate::currentDate().toString("yyyy_MM_dd"))
                .arg(QTime::currentTime().toString("hh_mm_ss_zzz"));

        if(file_global.isOpen())
            file_global.close();

        file_global.setFileName(logFileName);
        if (!file_global.open(QIODevice::Append |QIODevice::WriteOnly))
        {
            qDebug() << "Ошибка при открытии файла";
            return;
        }
    }


    else if(cmd == STOP_ESP){
        file_global.flush();
        file_global.close();
    }

    else if (cmd == SETTINGS_ESP) {

    }
    else if(cmd == SEARCH_ESP){

    }

    out.writeRawData(myHeader, 3);
    out << (quint8) cmd;
    out << (quint8) setG;
    out << (quint8) setHR;

    _sok_udp->writeDatagram(block, QHostAddress::Broadcast, port_ );

}
