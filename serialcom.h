#ifndef SERIALCOM_H
#define SERIALCOM_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMetaType>
#include <QVector>

struct _ATcmdESP{
    const QString cmdAccesPoint = "AT+CWSAP_DEF=\"ESP8266\",\"1234567890\",5,3,8,0";
    const QString cmdOffEcho = "ATE0";
    const QString cmdOnEcho = "ATE1";
    const QString cmdCIPMUX = "AT+CIPMUX=1";
    const QString cmdServer = "AT+CIPSERVER=1,8000";
    const QString cmdStartUDP = "AT+CIPSTART=1,\"UDP\",\"192.168.4.255\",8000,8000,2";
};

class SerialCom : public QObject
{
    Q_OBJECT

public:
    SerialCom(QObject *parent = nullptr);
    ~SerialCom();
    void SerialConnect();
    QList<QString> getSerPortInfo();
    void closePort();
    void settingESP();
public slots:
    void writeSerial(QString);
    void setSerPortName(QString);
signals:
    void messageProj(QString);
private slots:
    void onCmdReadyRead();
    void onDataReadyRead();
private:
    QSerialPort *m_serial;
    _ATcmdESP ATcmdESP07;
    int _pcktSize = 250 + 13;       //13 header, 250 data size

};

#endif // SERIALCOM_H
