#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMenu>
#include <QLineEdit>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    _devNumber = 6;
    maxCountGraphics_ = 255;
    ui->setupUi(this);

    WidthGraph = ui->lineWidth->text().toUInt();
    shiftGraph = ui->lineShift->text().toUInt();
    vecCntGraph_.resize(maxCountGraphics_+1);

    /* Set ToolBars */
    ui->toolBar->toggleViewAction()->setEnabled(false);

    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/image/openFile.png"));
    QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Открыть файл с измерениями"));
    connect(openAct, &QAction::triggered, this, &MainWindow::openFile);
    ui->toolBar->addAction(openAct);

    const QIcon settingsIcon = QIcon::fromTheme("settings-ico", QIcon(":/image/settings.ico"));
    QAction *setgtingsAct = new QAction(settingsIcon, tr("&Settings..."), this);
    //openAct->setShortcuts(QKeySequence::Open);
    setgtingsAct->setShortcut(QKeySequence(tr("CTRL+S")));  //settings
    setgtingsAct->setStatusTip(tr("Настройка устройств"));
    connect(setgtingsAct, &QAction::triggered, this, &MainWindow::settingsSlot);
    ui->toolBar->addAction(setgtingsAct);

    setRadButId();
    connect(ui->buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(onGroupButtonClicked(int)));
    connect(ui->buttonGroup_2, SIGNAL(buttonClicked(int)), this, SLOT(onGroupButtonClicked_2(int)));
    connect(ui->buttonGroup_3, SIGNAL(buttonClicked(int)), this, SLOT(onGroupButtonClicked_3(int)));
    connect(ui->buttonGroup_4, SIGNAL(buttonClicked(int)), this, SLOT(onGroupButtonClicked_4(int)));
    connect(ui->buttonGroup_5, SIGNAL(buttonClicked(int)), this, SLOT(onGroupButtonClicked_5(int)));
    connect(ui->buttonGroup_6, SIGNAL(buttonClicked(int)), this, SLOT(onGroupButtonClicked_6(int)));

    _listGraph.append(ui->customPlot1);
    _listGraph.append(ui->customPlot2);
    _listGraph.append(ui->customPlot3);
    _listGraph.append(ui->customPlot4);
    _listGraph.append(ui->customPlot5);
    _listGraph.append(ui->customPlot6);

    initGraphics();
    //test_plot();


    /* Init list measure */
    initListMeasure();
    /* TCP server */
    _server = new server();
    connect(_server, SIGNAL(newData(pointsDevices*)), this, SLOT(dataProccesing(pointsDevices*)));

    ui->layoutColorScale->addWidget(new ColorScale());

}

MainWindow::~MainWindow()
{
    delete ui;
    for(int i=0; i < _devNumber; i++){
       delete _listADC1.at(i);
       delete _listADC2.at(i);
       delete _listAccX.at(i);
       delete _listAccY.at(i);
       delete _listAccZ.at(i);
       delete _listTsens.at(i);
    }
}

void MainWindow::projMessage(QString textMessage)
{
    ui->textEdit->insertPlainText(textMessage);
    //ui->textEdit->append(textMessage);
}


void MainWindow::dataProccesing(pointsDevices *listDevice)
{
    uint8_t device = listDevice->numDevices;
    /*заполняем _list данными */
    for(int i=0; i < listDevice->ADC1.size(); i++){
        _listADC1 [device]->append(listDevice->ADC1);
        _listADC2 [device]->append(listDevice->ADC2);
        _listAccX [device]->append(listDevice->accX);
        _listAccY [device]->append(listDevice->accY);
        _listAccZ [device]->append(listDevice->accZ);
        _listTsens[device]->append(listDevice->Tsens);
    }

    if(flagRadButtons[device] == sRadioADC1){
        _listGraph.at(device)->yAxis->setLabel( "ADC1" );
        _listGraph.at(device)->graph()->setData(X, *_listADC1.at(device));
    }
    else if(flagRadButtons[device] == sRadioADC2){
        _listGraph.at(device)->yAxis->setLabel( "ADC2" );
        _listGraph.at(device)->graph()->setData(X, *_listADC2.at(device));
    }
    else if(flagRadButtons[device] == sRadioAccX){
        _listGraph.at(device)->yAxis->setLabel( "AccX" );
        _listGraph.at(device)->graph()->setData(X, *_listAccX.at(device));
    }
    else if(flagRadButtons[device] == sRadioAccY){
        _listGraph.at(device)->yAxis->setLabel( "AccY" );
        _listGraph.at(device)->graph()->setData(X, *_listAccY.at(device));
    }
    else if(flagRadButtons[device] == sRadioAccZ){
        _listGraph.at(device)->yAxis->setLabel( "AccZ" );
        _listGraph.at(device)->graph()->setData(X, *_listAccZ.at(device));
    }
    else if(flagRadButtons[device] == sRadioTsens){
        _listGraph.at(device)->yAxis->setLabel( "T" );
        _listGraph.at(device)->graph()->setData(X, *_listTsens.at(device));
    }

    _listGraph.at(device)->rescaleAxes();      // Масштабируем график по данным
    _listGraph.at(device)->replot();           // Отрисовываем график
}

void MainWindow::openFile()
{
    QString path_file = "C:/Test";
    QString file_name = QFileDialog::getOpenFileName(this, "Открыть файл с измерениями",
                                                     path_file, "Txt File(*.txt)");
    emit setFileName(file_name);
}

void MainWindow::settingsSlot()
{

}

void MainWindow::initGraphics()
{
//    QFont legendFont = font();
//    legendFont.setPointSize(8);

    for(int i=0; i<_devNumber; i++){
        _listGraph.at(i)->addGraph(); // blue line
        _listGraph.at(i)->graph(0)->setPen(QPen(QColor(0, 0, 100)));

        mColorMap1 = new QCPColorMap(_listGraph.at(i)->xAxis, _listGraph.at(i)->yAxis);
        mColorMap1->rescaleDataRange(true);
        //mColorMap1->data()->setRange(QCPRange(0, 50), QCPRange(0, 1));
        mColorMap1->data()->setSize(0, 5);
        mColorMap1->setInterpolate(0);
        mColorScale1 = new QCPColorScale(_listGraph.at(i));
        _listGraph.at(i)->plotLayout()->addElement(0, 1, mColorScale1);
        mColorMap1->setColorScale(mColorScale1);

        QCPColorGradient *mGradient = new QCPColorGradient();
        mColorMap1->setGradient(mGradient->gpJet );
        mGradient->setLevelCount(20);
        mColorMap1->setDataRange(QCPRange(0, maxCountGraphics_));   //range тепловой карты

        _listGraph.at(i)->rescaleAxes();
        _listGraph.at(i)->replot();

//        _listGraph.at(i)->legend->setVisible(true);
//        _listGraph.at(i)->legend->setFont(legendFont);
//        _listGraph.at(i)->legend->setSelectedFont(legendFont);
//        _listGraph.at(i)->legend->setIconBorderPen(Qt::NoPen);
//        _listGraph.at(i)->legend->setSelectableParts(QCPLegend::spNone);
//        _listGraph.at(i)->legend->setIconSize(9, 9);
//        _listGraph.at(i)->graph()->setName(QString("Модуль %1").arg(i+1));
    }
}

void MainWindow::ReinitGraphic(int numDev)
{
    _listGraph.at(numDev)->clearGraphs();

    _listGraph.at(numDev)->addGraph(); // blue line
    _listGraph.at(numDev)->graph(0)->setPen(QPen(QColor(0, 0, 100)));

    mColorMap1 = new QCPColorMap(_listGraph.at(numDev)->xAxis, _listGraph.at(numDev)->yAxis);
    mColorMap1->rescaleDataRange(true);
    //mColorMap1->data()->setRange(QCPRange(0, 50), QCPRange(0, 1));
    mColorMap1->data()->setSize(0, 5);
    mColorMap1->setInterpolate(0);
    mColorScale1 = new QCPColorScale(_listGraph.at(numDev));
    _listGraph.at(numDev)->plotLayout()->addElement(0, 1, mColorScale1);
    mColorMap1->setColorScale(mColorScale1);

    QCPColorGradient *mGradient = new QCPColorGradient();
    mColorMap1->setGradient(mGradient->gpJet );
    mGradient->setLevelCount(20);
    mColorMap1->setDataRange(QCPRange(0, maxCountGraphics_));   //range тепловой карты

    _listGraph.at(numDev)->rescaleAxes();
    _listGraph.at(numDev)->replot();
}

void MainWindow::test_plot(){

    ui->customPlot1->addGraph();
    ui->customPlot1->graph(0)->setPen(QPen(QColor(0, 0, 100)));
    mColorMap1 = new QCPColorMap(ui->customPlot1->xAxis, ui->customPlot1->yAxis);
    mColorMap1->rescaleDataRange(true);
    mColorMap1->data()->setRange(QCPRange(0, 50), QCPRange(0, 271));
    mColorMap1->data()->setSize(0, 5);
    mColorMap1->setInterpolate(0);
    mColorScale1 = new QCPColorScale(ui->customPlot1);
    ui->customPlot1->plotLayout()->addElement(0, 1, mColorScale1);
    mColorMap1->setColorScale(mColorScale1);
    QCPColorGradient *mGradient = new QCPColorGradient();
    mColorMap1->setGradient(mGradient->gpJet );
    mGradient->setLevelCount(20);
    mColorMap1->setDataRange(QCPRange(0, maxCountGraphics_));
    ui->customPlot1->rescaleAxes();
    ui->customPlot1->replot();

    qDebug() << "key " << ui->customPlot1->graph(0)->data()->dataRange();
    qDebug() << "value " << ui->customPlot1->graph(0)->data()->at(1)->value;
}

void MainWindow::setRadButId()
{
    ui->buttonGroup->setId(ui->radioADC1, 0);
    ui->buttonGroup->setId(ui->radioADC2, 1);
    ui->buttonGroup->setId(ui->radioAccX, 2);
    ui->buttonGroup->setId(ui->radioAccY, 3);
    ui->buttonGroup->setId(ui->radioAccZ, 4);
    ui->buttonGroup->setId(ui->radioTsense, 5);

    ui->buttonGroup_2->setId(ui->radioADC1_2, 0);
    ui->buttonGroup_2->setId(ui->radioADC2_2, 1);
    ui->buttonGroup_2->setId(ui->radioAccX_2, 2);
    ui->buttonGroup_2->setId(ui->radioAccY_2, 3);
    ui->buttonGroup_2->setId(ui->radioAccZ_2, 4);
    ui->buttonGroup_2->setId(ui->radioTsense_2, 5);

    ui->buttonGroup_3->setId(ui->radioADC1_3, 0);
    ui->buttonGroup_3->setId(ui->radioADC2_3, 1);
    ui->buttonGroup_3->setId(ui->radioAccX_3, 2);
    ui->buttonGroup_3->setId(ui->radioAccY_3, 3);
    ui->buttonGroup_3->setId(ui->radioAccZ_3, 4);
    ui->buttonGroup_3->setId(ui->radioTsense_3, 5);

    ui->buttonGroup_4->setId(ui->radioADC1_4, 0);
    ui->buttonGroup_4->setId(ui->radioADC2_4, 1);
    ui->buttonGroup_4->setId(ui->radioAccX_4, 2);
    ui->buttonGroup_4->setId(ui->radioAccY_4, 3);
    ui->buttonGroup_4->setId(ui->radioAccZ_4, 4);
    ui->buttonGroup_4->setId(ui->radioTsense_4, 5);

    ui->buttonGroup_5->setId(ui->radioADC1_5, 0);
    ui->buttonGroup_5->setId(ui->radioADC2_5, 1);
    ui->buttonGroup_5->setId(ui->radioAccX_5, 2);
    ui->buttonGroup_5->setId(ui->radioAccY_5, 3);
    ui->buttonGroup_5->setId(ui->radioAccZ_5, 4);
    ui->buttonGroup_5->setId(ui->radioTsense_5, 5);

    ui->buttonGroup_6->setId(ui->radioADC1_6, 0);
    ui->buttonGroup_6->setId(ui->radioADC2_6, 1);
    ui->buttonGroup_6->setId(ui->radioAccX_6, 2);
    ui->buttonGroup_6->setId(ui->radioAccY_6, 3);
    ui->buttonGroup_6->setId(ui->radioAccZ_6, 4);
    ui->buttonGroup_6->setId(ui->radioTsense_6, 5);
}

/* Отправляем команду ESP */
void MainWindow::on_pbSendESP_clicked()
{
    QString cmd = ui->cbCmdESP->currentText();
    if(cmd == "START"){
        m_cmdESP = START_ESP;
        ui->lineShift->setEnabled(false);
        ui->lineWidth->setEnabled(false);
        ui->pbChangeShift->setEnabled(false);

    }
    else if(cmd == "STOP"){
        m_cmdESP = STOP_ESP;
    }
    emit cmdEspALL(m_cmdESP);


}

void MainWindow::changeStrRadBut(int idGroup, int numButton)
{
    switch(numButton){
    case 0:
        flagRadButtons[idGroup] = sRadioADC1;
        break;
    case 1:
        flagRadButtons[idGroup] = sRadioADC2;

        break;
    case 2:
        flagRadButtons[idGroup] = sRadioAccX;

        break;
    case 3:
        flagRadButtons[idGroup] = sRadioAccY;

        break;
    case 4:
        flagRadButtons[idGroup] = sRadioAccZ;

        break;
    case 5:
        flagRadButtons[idGroup] = sRadioTsens;
        break;
    }
}

void MainWindow::onGroupButtonClicked(int numButton)
{
    changeStrRadBut(0, numButton);
}

void MainWindow::onGroupButtonClicked_2(int numButton)
{
    changeStrRadBut(1, numButton);
}

void MainWindow::onGroupButtonClicked_3(int numButton)
{
    changeStrRadBut(2, numButton);
}

void MainWindow::onGroupButtonClicked_4(int numButton)
{
    changeStrRadBut(3, numButton);
}

void MainWindow::onGroupButtonClicked_5(int numButton)
{
    changeStrRadBut(4, numButton);
}

void MainWindow::onGroupButtonClicked_6(int numButton)
{
    changeStrRadBut(5, numButton);
}

void MainWindow::initListMeasure()
{
    /* если проинициализировано уже */
    if(flagRadButtons.count()){
        flagRadButtons.clear();
        X.clear();
        for(int i=0; i < _devNumber; i++){

           delete _listADC1.at(i);
           delete _listADC2.at(i);
           delete _listAccX.at(i);
           delete _listAccY.at(i);
           delete _listAccZ.at(i);
           delete _listTsens.at(i);
         }
    }

    for(int i=0; i < _devNumber; i++)
    {
        flagRadButtons.append("ADC1");
       _listADC1.append((new QVector <double>[WidthGraph]));
       _listADC2.append((new QVector <double>[WidthGraph]));
       _listAccX.append((new QVector <double>[WidthGraph]));
       _listAccY.append((new QVector <double>[WidthGraph]));
       _listAccZ.append((new QVector <double>[WidthGraph]));
       _listTsens.append((new QVector <double>[WidthGraph]));
    }
    for(int i=0; i < WidthGraph; i++) X.append(i);
}

void MainWindow::on_pbChangeShift_clicked()
{
    WidthGraph = ui->lineWidth->text().toUInt();
    shiftGraph = ui->lineShift->text().toUInt();
    initListMeasure();
}
