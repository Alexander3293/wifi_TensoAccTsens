#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QMetaType>
#include <QDataStream>
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
    STOP_ESP = 0,
    START_ESP = 1,
    SETTINGS_ESP = 2,
    SEARCH_ESP = 3
}cmdESP;

class server: public QObject
{
public:

    Q_OBJECT
public:
    server(QObject *parent = 0);
    ~server();
    void initUDP();

public slots:

    void slotDataRead();
    void cmdEspAllSlot(cmdESP cmd, uint setG, uint setScale);
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
    //QString myHeader  = "ff00dd";
    QString devReadyHeader  = "edaaff";
    char myHeader[3];
    uint16_t OneDatagramSize = 250;
    quint32 counterDatagram;
    uint32_t dataDatagramlen;
    int deviceId;



};

Q_DECLARE_METATYPE(pointsDevices);
#endif // SERVER_H
