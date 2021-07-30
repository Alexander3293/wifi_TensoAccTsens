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


}

server::~server()
{
    delete _sok_udp;
    if(file_global.isOpen())
        file_global.close();
}

void server::slotConnect()
{
    _sok_udp = new QUdpSocket();
    host_ = QHostAddress("192.168.136.1");
    port_ = 8000;
    //_sok_udp->bind(host_, 8000);
    _sok_udp->bind(QHostAddress::Any, 8000);
//    this->port_ = 1500;


    connect(_sok_udp, &QUdpSocket::readyRead, this, &server::slotDataRead);
}

void server::slotDataRead()
{
    QByteArray datagram;
    bool ok;

    while(_sok_udp->bytesAvailable() <= OneDatagramSize){
        datagram = _sok_udp->read(OneDatagramSize);

        dataDatagramlen =  datagram.length()*2;
        counterDatagram = 0;
        deviceId = -1;

        QString str_data = datagram.toHex();

        file_global.write(str_data.toUtf8());       //Write to the File

        QString tmp_str;

        tmp_str = str_data.mid(counterDatagram, 8);
        if(tmp_str.startsWith(espHeader, Qt::CaseSensitivity::CaseInsensitive)) {
            deviceId = str_data.rightRef(2).toInt();
            counterDatagram += 8;
        }
        else return;

        while(counterDatagram < dataDatagramlen){

            listPointDevices.at(deviceId)->ADC1.append(str_data.midRef(counterDatagram, 4).toInt(&ok, 16));
            listPointDevices.at(deviceId)->ADC1.append(str_data.midRef(counterDatagram+4, 4).toInt(&ok, 16));
            listPointDevices.at(deviceId)->accX.append(str_data.midRef(counterDatagram+8, 4).toInt(&ok, 16));
            listPointDevices.at(deviceId)->accY.append(str_data.midRef(counterDatagram+12, 4).toInt(&ok, 16));
            listPointDevices.at(deviceId)->accZ.append(str_data.midRef(counterDatagram+16, 4).toInt(&ok, 16));
            listPointDevices.at(deviceId)->Tsens.append(str_data.midRef(counterDatagram+20, 4).toInt(&ok, 16));

            counterDatagram += 24;
        }

        emit(listPointDevices.at(deviceId));    /*Передать измерения */

        listPointDevices.at(deviceId)->ADC1.clear();
        listPointDevices.at(deviceId)->ADC2.clear();
        listPointDevices.at(deviceId)->accX.clear();
        listPointDevices.at(deviceId)->accY.clear();
        listPointDevices.at(deviceId)->accZ.clear();
        listPointDevices.at(deviceId)->Tsens.clear();

    }
}

void server::cmdEspAllSlot(cmdESP cmd)
{
    if(cmd == START_ESP){
        /*create file, to write data from ESP */
        logFileName  = QString("D:/test_server/Log_%1__%2.txt")
                .arg(QDate::currentDate().toString("yyyy_MM_dd"))
                .arg(QTime::currentTime().toString("hh_mm_ss_zzz"));

        file_global.setFileName(logFileName);
        if (!file_global.open(QIODevice::Append |QIODevice::WriteOnly))
        {
            qDebug() << "Ошибка при открытии файла";
            return;
        }
    }


    else if(cmd == STOP_ESP){
        file_global.close();
    }

    _sok_udp->writeDatagram(QByteArray::number(cmd), QHostAddress::Broadcast, 8001 );
}
