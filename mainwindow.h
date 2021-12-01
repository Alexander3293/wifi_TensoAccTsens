#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "server.h"
#include <qcustomplot.h>
#include "colorscale.h"
#include "fileprocessing.h"
#include "fileform.h"

typedef struct{
    QCPGraph* graph;
    QColor color;
    QMultiMap <int, int16_t> mapValueGraph;  //int - ось X, int16_t - значения
}graphColor;

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
    void cmdEspALL(cmdESP, uint, uint);
    void setFileName(QString);
public slots:
    void projMessage(QString);
private slots:
    void dataProccesing(pointsDevices* listDevice);
    void openFile();
    void graphicSettings();
    void settingsSlot();
    void startSlot();
    void stopSlot();
    void searchSlot();
    void on_pbSendESP_clicked();
    void onGroupButtonClicked(int numButton);
    void onGroupButtonClicked_2(int numButton);
    void onGroupButtonClicked_3(int numButton);
    void onGroupButtonClicked_4(int numButton);
    void onGroupButtonClicked_5(int numButton);
    void onGroupButtonClicked_6(int numButton);
    void getSettingMeasure(pointsMeasure_ *);
    void on_pbChangeShift_clicked();

private:
    int checkGraphics(const uint8_t devNum, QVector <double> *data, double accuracy);
    void addColorGraphPoint(const uint8_t devNum,const int keyX, const double valYdata, const double accuracy);
    void GraphProcessing(const uint8_t numDev, QVector<double> *data, const double acc);
    void GraphProcessingV2(const uint8_t numDev, const uint16_t numGraph, QVector<double> *data, const double acc);

    void setRadButId();
    void changeStrRadBut(int idGroup, int numButton);
    void test_plot();
    void initGraphics();
    void ReinitGraphic(int numDev);
    void initListMeasure();         //инициализация множества массивов, где хранятся данные
    void initColorGraphs();
    void deInitColorGraphs();
    QString getTextRadBut(int);

    Ui::MainWindow *ui;
    cmdESP m_cmdESP;
    server *_server;
    uint32_t _pcktlen, counterTcpPckt;
    uint32_t counterX=0;
    uint8_t _devNumber;
    uint8_t numDevice;
    QString fileName_;
    QString dirToSave_;
    uint16_t WidthGraph;
    uint16_t shiftGraph;
    uint8_t g_;
    uint8_t hr_;

    QList<QVector<double>*> _listData;  //Тут хранятся данные с 6 графиков для построения
    QVector<uint> listShift_;
    QVector<double> X;

    QStringList flagRadButtons;
    QList<QButtonGroup*> butGrList;
    QString sRadioADC1 = "ADC1";
    QString sRadioADC2 = "ADC2";
    QString sRadioAccX = "AccX";
    QString sRadioAccY = "AccY";
    QString sRadioAccZ = "AccZ";
    QString sRadioTsens = "T";

    QList<QCustomPlot* > _listGraph;
    QList<QVector<uint8_t>*> vecCntGraph_;//содержит количество повторений графиков
    int currentIndexGraph[6];
    uint maxColorGraphics_;              //количество графиков для цвета
    int maxGraphOnScene;               //Количестов графиков для построения
    double accuracyCompare_;             //точность сравнения
    bool beginGraph[6];
    QList <QList <graphColor*>> listColorGraph; //list с цветами графиков по 256 вкаждом из 6 графиков

    QCPColorMap* mColorMap1;
    QCPMarginGroup* mMarginGroup;
    QCPColorScale* mColorScale1;
    QPen pen;

    /*widget Color */
    ColorScale * wColorScale;
    QColor curColor;

    /* File measure */
    FileProcessing* fileMeasure;
    fileForm* fileUI;
    QFile file_global;

};
#endif // MAINWINDOW_H
