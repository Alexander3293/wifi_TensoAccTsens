#include "serialcom.h"
#include <QList>
#include <QThread>
SerialCom::SerialCom(QObject *parent) : QObject(parent)
{
    m_serial = new QSerialPort();
}

SerialCom::~SerialCom()
{
    delete m_serial;
}

void SerialCom::SerialConnect()
{
    m_serial->setBaudRate(115200);
    m_serial->setDataBits(QSerialPort::Data8);
    m_serial->setParity(QSerialPort::NoParity);
    m_serial->setStopBits(QSerialPort::OneStop);
    m_serial->setFlowControl(QSerialPort::NoFlowControl);
    m_serial->open(QIODevice::ReadWrite);
}

QList<QString> SerialCom::getSerPortInfo()
{
    QList<QString> serPortName;
    QSerialPortInfo portInfo;
    QList<QSerialPortInfo> com_ports = QSerialPortInfo::availablePorts();
    foreach(portInfo, com_ports)
    {
       serPortName.append(portInfo.portName());
    }
    return serPortName;
}

void SerialCom::closePort()
{
    m_serial->close();
    disconnect(m_serial, SIGNAL(readyRead()), this, SLOT(onCmdReadyRead()));
}

void SerialCom::settingESP()
{
    m_serial->write(ATcmdESP07.cmdCIPMUX.toUtf8() +'\r'+ '\n');
    m_serial->waitForReadyRead(-1);
    while(m_serial->bytesAvailable() > 0)
    {
        QByteArray message = m_serial->readAll();
        emit messageProj(message);
    }
    QThread::msleep(100);

    m_serial->write(ATcmdESP07.cmdServer.toUtf8() +'\r'+ '\n');
    m_serial->waitForReadyRead(-1);
    while(m_serial->bytesAvailable() > 0)
    {
        QByteArray message = m_serial->readAll();
        emit messageProj(message);
    }
    QThread::msleep(100);

    m_serial->write(ATcmdESP07.cmdStartUDP.toUtf8() +'\r'+ '\n');
    m_serial->waitForReadyRead(-1);
    while(m_serial->bytesAvailable() > 0)
    {
        QByteArray message = m_serial->readAll();
        emit messageProj(message);
    }
    QThread::msleep(100);
    connect(m_serial, SIGNAL(readyRead()), this, SLOT(onCmdReadyRead()));
}

void SerialCom::setSerPortName(QString portName)
{
    m_serial->setPortName(portName);
}


void SerialCom::writeSerial(QString textSerial)
{
    m_serial->write(textSerial.toUtf8() +'\r'+ '\n');
}

void SerialCom::onCmdReadyRead()
{
    while(m_serial->bytesAvailable() > 0)
    {
        QString message = m_serial->readAll();
        emit messageProj(message);
    }
}

void SerialCom::onDataReadyRead()
{
    while(m_serial->bytesAvailable() > _pcktSize)
    {
        QString message = m_serial->read(_pcktSize);
        message.remove(QRegExp("[\\n\\t\\r]"));

        /* тут запись в файл сделать */

        /*---------------------------*/

        if(message.startsWith("+IPD,",Qt::CaseInsensitive)){

        }
    }
}

/*
 *
 * AT+CWSAP_DEF="ESP8266","1234567890",5,3,8,0          //точка доступа, пароль, channel id, WPA_WPA2_PSK, max client 8, not broadcast
 *
 * ATE0                                                 // off echo
 * AT+CIPMUX=1
 * AT+CIPSERVER=1,8000
 * AT+CIPSTART=1,"UDP","192.168.4.1",8000,8000,2
 *
 */
