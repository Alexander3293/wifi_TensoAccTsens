#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QMetaType>

#include <QFile>
#include <QTime>

typedef struct
{
    uint8_t numPckt;
    uint8_t	numDevices;
    QVector<double> ADC1;
    QVector<double> ADC2;
    QVector<double> Tsens;
    QVector<double> accX;
    QVector<double> accY;
    QVector<double> accZ;

} pointsDevices;

typedef enum {
    START_ESP = 0,
    STOP_ESP = 1
}cmdESP;

class server: public QObject
{
public:

    Q_OBJECT
public:
    server(QObject *parent = 0);
    ~server();

public slots:
    void slotConnect();
    void slotDataRead();
    void cmdEspAllSlot(cmdESP);
signals:
    void newData(pointsDevices* data);
private:
    QUdpSocket *_sok_udp; //сокет
    QList<pointsDevices* > listPointDevices;
    QFile file_global;      // for the correct close file
    QString logFileName;    // Name of the global file
    QHostAddress host_;
    int port_;

    QString espHeader = "ed00ff";
    uint16_t OneDatagramSize = 250;
    quint32 counterDatagram;
    uint32_t dataDatagramlen;
    int deviceId;


};

Q_DECLARE_METATYPE(pointsDevices);
#endif // SERVER_H
