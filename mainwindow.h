#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "server.h"
#include <qcustomplot.h>
#include "colorscale.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void cmdEspALL(cmdESP);
    void setFileName(QString);
public slots:
    void projMessage(QString);
private slots:
    void dataProccesing(pointsDevices* listDevice);
    void openFile();
    void settingsSlot();
    void on_pbSendESP_clicked();
    void onGroupButtonClicked(int numButton);
    void onGroupButtonClicked_2(int numButton);
    void onGroupButtonClicked_3(int numButton);
    void onGroupButtonClicked_4(int numButton);
    void onGroupButtonClicked_5(int numButton);
    void onGroupButtonClicked_6(int numButton);

    void on_pbChangeShift_clicked();

private:
    void initGraphics();
    void ReinitGraphic(int numDev);
    void setRadButId();
    void changeStrRadBut(int idGroup, int numButton);
    void test_plot();
    void initListMeasure();         //инициализация множества массивов, где хранятся данные

    Ui::MainWindow *ui;
    cmdESP m_cmdESP;
    server *_server;
    uint32_t _pcktlen, counterTcpPckt;
    uint32_t counterX=0;
    uint8_t _devNumber;
    uint16_t WidthGraph;
    uint16_t shiftGraph;

    QList<QVector<double>*> _listADC1;
    QList <QVector<double>*> _listADC2;
    QList <QVector<double>*> _listTsens;
    QList <QVector<double>*> _listAccX;
    QList <QVector<double>*> _listAccY;
    QList <QVector<double>*> _listAccZ;
    QVector<double> X;

    QStringList flagRadButtons;
    QString sRadioADC1 = "ADC1";
    QString sRadioADC2 = "ADC2";
    QString sRadioAccX = "AccX";
    QString sRadioAccY = "AccY";
    QString sRadioAccZ = "AccZ";
    QString sRadioTsens = "T";

    QList<QCustomPlot* > _listGraph;
    QVector<uint8_t> vecCntGraph_;  //содержит количество повторений графиков (пока что для 1 графика только)
    uint maxCountGraphics_;         //максимальное количество графиков
    double accuracyCompare_;        //точность сравнения

    QCPColorMap* mColorMap1;
    QCPMarginGroup* mMarginGroup;
    QCPColorScale* mColorScale1;


};
#endif // MAINWINDOW_H
