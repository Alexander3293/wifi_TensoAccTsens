#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMenu>
#include <QLineEdit>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    _devNumber = 6;
    maxColorGraphics_ = 30;
    maxGraphOnScene = 50;
    ui->setupUi(this);

    wColorScale = new ColorScale();
    ui->layoutColorScale->addWidget(wColorScale);

    WidthGraph = ui->lineWidth->text().toUInt();
    shiftGraph = ui->lineShift->text().toUInt();
    listShift_.resize(_devNumber);

    /* Set ToolBars */
    ui->toolBar->toggleViewAction()->setEnabled(false);

    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/image/openFile.png"));
    QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Открыть файл с измерениями"));
    connect(openAct, &QAction::triggered, this, &MainWindow::openFile);
    ui->toolBar->addAction(openAct);

    const QIcon graphicIcon = QIcon::fromTheme("graph-open", QIcon(":/image/graphic.png"));
    QAction *graphicAct = new QAction(graphicIcon, tr("&Graphic..."), this);
    graphicAct->setShortcuts(QKeySequence::Open);
    graphicAct->setStatusTip(tr("Настройка графика"));
    connect(graphicAct, &QAction::triggered, this, &MainWindow::graphicSettings);
    ui->toolBar->addAction(graphicAct);

    const QIcon settingsIcon = QIcon::fromTheme("settings-ico", QIcon(":/image/settings.ico"));
    QAction *setgtingsAct = new QAction(settingsIcon, tr("&Settings..."), this);
    //openAct->setShortcuts(QKeySequence::Open);
    setgtingsAct->setShortcut(QKeySequence(tr("CTRL+S")));  //settings
    setgtingsAct->setStatusTip(tr("Настройка устройств"));
    connect(setgtingsAct, &QAction::triggered, this, &MainWindow::settingsSlot);
    ui->toolBar->addAction(setgtingsAct);

    const QIcon startIcon = QIcon::fromTheme("start-ico", QIcon(":/image/start.png"));
    QAction *startAct = new QAction(startIcon, tr("&Play..."), this);
    //openAct->setShortcuts(QKeySequence::Open);
    startAct->setShortcut(QKeySequence(tr("CTRL+P")));  //start
    startAct->setStatusTip(tr("Начать сбор данных"));
    connect(startAct, &QAction::triggered, this, &MainWindow::startSlot);
    ui->toolBar->addAction(startAct);

    const QIcon stopIcon = QIcon::fromTheme("stop-ico", QIcon(":/image/stop.png"));
    QAction *stopAct = new QAction(stopIcon, tr("&Stop..."), this);
    //openAct->setShortcuts(QKeySequence::Open);
    //startAct->setShortcut(QKeySequence(tr("CTRL+P")));  //start
    stopAct->setStatusTip(tr("Остановить сбор данных"));
    connect(stopAct, &QAction::triggered, this, &MainWindow::stopSlot);
    ui->toolBar->addAction(stopAct);

    const QIcon searchIcon = QIcon::fromTheme("start-ico", QIcon(":/image/search.png"));
    QAction *searchAct = new QAction(searchIcon, tr("&Search..."), this);
    //openAct->setShortcuts(QKeySequence::Open);
    //searchAct->setShortcut(QKeySequence(tr("CTRL+P")));  //start
    searchAct->setStatusTip(tr("Готовность устройств"));
    connect(searchAct, &QAction::triggered, this, &MainWindow::searchSlot);
    ui->toolBar->addAction(searchAct);

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
    initColorGraphs();
    /* Init list measure */
    initListMeasure();
    //test_plot();
    /* TCP server */
    _server = new server();
    _server->initUDP();
    disconnect(_server, SIGNAL(newData(pointsDevices*)), this, SLOT(dataProccesing(pointsDevices*)));
    connect(this, SIGNAL(cmdEspALL(cmdESP, uint, uint)), _server, SLOT(cmdEspAllSlot(cmdESP, uint, uint)));
    fileUI = new fileForm();
    //fileUI->show();
    connect(fileUI, SIGNAL(chooseMeasureModules(pointsMeasure_*)), this , SLOT(getSettingMeasure(pointsMeasure_*)));
    //connect(this, SIGNAL(setFileName(QString)), SLOT(getFileName(QString)));
}

MainWindow::~MainWindow()
{
    delete ui;
    for(int i=0; i < _devNumber; i++)
       delete _listData.at(i);
}

void MainWindow::projMessage(QString textMessage)
{
    ui->textEdit->insertPlainText(textMessage);
    //ui->textEdit->append(textMessage);
}

/* Проверяем график на совпадение, если -1, то новый график строить */
int MainWindow::checkGraphics(const uint8_t devNum, QVector<double> *data, double accuracy)
{
    uint8_t cntGraph = 0;
    double val = 0;
    bool graphBreak = false;

    qDebug() << "check Graphics " << _listGraph[devNum]->graphCount();
    if(_listGraph[devNum]->graphCount() == 1)
        return -1;
    /* Перебираем графики cntGraph и их значения сравниваем */
    for(cntGraph=0; cntGraph < _listGraph[devNum]->graphCount(); cntGraph++){
        graphBreak = false;
        for(uint cnt=0; cnt < WidthGraph; cnt++){
            val = _listGraph[devNum]->graph(cntGraph)->data()->at(cnt)->value;
            //Если число больше точности, к новому графику переходим
            if(std::abs(val - data->at(cnt)) > accuracy){
                graphBreak = true;
                break;
            }
        }
        if(graphBreak == false){
            qDebug() << "Sovpali graphics " << cntGraph;
            return cntGraph;
        }
    }
   return -1;
}

void MainWindow::addColorGraphPoint(const uint8_t devNum,const int keyX,const double valYdata, const double accuracy)
{
    /*
     * 1. Ищем совпадение графиков
     * 2. Если есть, то проверяем есть ли уже такой ключ-значение в списке
     * 3. Пробегаем по графикам, пока не найдем
     *
     */
    /* Данные (x и y), полученные после сравнения графиков, сравнить с map*
     * TODO: добавить позже */
    /* От графика получили какое-то значение после сравнения */
    /* Это значение нужно сравнить со всеми ключами :) */
        for(uint cnt_list_graph_color = 0; cnt_list_graph_color < maxColorGraphics_; cnt_list_graph_color++){
            uint cnt_values = listColorGraph[devNum].at(cnt_list_graph_color)->mapValueGraph.count(keyX);
            if(cnt_values == 0){
                listColorGraph[devNum].at(cnt_list_graph_color)->mapValueGraph.insert(keyX, valYdata);
                listColorGraph[devNum].at(cnt_list_graph_color)->graph->addData(keyX, valYdata);
                qDebug() << "key_values" << keyX << valYdata;
                break;
            }
            /* Сравниваем значение с заданной точностью со значением ключа */
            else {
                /* список с ключами, которые проверяем */
                QList<int16_t> list_val = listColorGraph[devNum].at(cnt_list_graph_color)->mapValueGraph.values(keyX);
                bool next_graph = false;
                while(cnt_values > 0){
                    cnt_values--;
                    if(std::abs(list_val.at(cnt_values) - valYdata) < accuracy){
                        next_graph = true;
                        break;  //out while(cnt_values < 0)
                    }
                }
                if(next_graph == false){
                    listColorGraph[devNum].at(cnt_list_graph_color)->mapValueGraph.insert(keyX, valYdata);
                    listColorGraph[devNum].at(cnt_list_graph_color)->graph->addData(keyX, valYdata);
                    qDebug() << "key_values" << keyX << valYdata;
                    break;      //out  for(int cnt_list_graph_color = 0; cnt_list_graph_color < 256; cnt_list_graph_color++)
                }
            }
        }
}

void MainWindow::GraphProcessing(const uint8_t numDev, QVector<double> *data, const double acc)
{
    int tmpGraph = checkGraphics(numDev, data, acc);
    qDebug() << "tmpGraph " << tmpGraph;
    qDebug() << "dev Number" << numDev;

    if(tmpGraph == -1){
        currentIndexGraph[numDev]++;
        qDebug() << " currentIndex" << currentIndexGraph[numDevice];
        _listGraph.at(numDev)->addGraph();
        _listGraph.at(numDev)->graph(currentIndexGraph[numDev])->setData(X, *_listData.at(numDev));
    }
    else{
        /* Увеличим количество графиков с такими данными */
        uint8_t* dataVec  = vecCntGraph_[numDev]->data();
        dataVec+=tmpGraph;
        (*dataVec)++;
        /*GOTO: get Color */
        maxColorGraphics_ = 30;
        curColor = wColorScale->getColor((double)(*dataVec)/maxColorGraphics_);

        pen.setColor(curColor);
        _listGraph.at(numDev)->graph(tmpGraph)->setPen(pen);
        _listGraph.at(numDev)->replot();
    }
}

void MainWindow::GraphProcessingV2(const uint8_t numDev, const uint16_t numGraph, QVector<double> *data, const double acc)
{
    uint8_t cntGraph = 0;
    double valGraph = 0;
    double valCurrentGraph = 0;
    int current_cnt_graphics = _listGraph[numDev]->graphCount() - maxColorGraphics_;
    /* Перебираем графики cntGraph и их значения сравниваем */
    for(uint cnt=0; cnt < WidthGraph; cnt++){
        for(cntGraph=0; cntGraph < current_cnt_graphics; cntGraph++){
            if(numGraph == cntGraph)
                continue;
            /* Если график еще не заполнен, данные пусты */
            if(_listGraph[numDev]->graph(cntGraph)->data()->isEmpty()){
                continue;
            }
            valGraph = _listGraph[numDev]->graph(cntGraph)->data()->at(cnt)->value;
            valCurrentGraph = data->at(cnt);
//            qDebug() <<_listGraph[devNum]->graph(cntGraph)->data()->at(cnt)->value;
//            qDebug() << "graph0" << cntGraph << "graph1" << numGraph << "cnt" << cnt << "raznost" << std::abs(valGraph - data->at(cnt));
            qDebug() << "val" << valGraph << "data->at(cnt)" << data->at(cnt);
            if(std::abs(valGraph - valCurrentGraph) <= acc){
                addColorGraphPoint(numDev, cnt, valCurrentGraph, acc);
                break;
            }
        }
    }
     _listGraph.at(numDev)->replot();
}
void MainWindow::dataProccesing(pointsDevices *listDevice)
{
    numDevice = listDevice->numDevices;
    /*заполняем _list данными в зависимости от выбранного radioButton*/
    if(flagRadButtons[numDevice] == sRadioADC1){
        for(int i=0; i < listDevice->ADC1.size(); i++){

            if(listShift_[numDevice]!=0)
                listShift_[numDevice]--;
            else{
                _listData [numDevice]->append(listDevice->ADC1.at(i));
            }

            if(beginGraph[numDevice] != true){
                 _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice])->addData(X, *_listData[numDevice]);
                 _listGraph.at(numDevice)->rescaleAxes();      // Масштабируем график по данным
                 _listGraph.at(numDevice)->replot();           // Отрисовываем график
            }
            if(_listData [numDevice]->size() >= WidthGraph){
                pen.setColor(QColor(0, 0, 100));
                 pen.setWidth(1);
                _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice])->setPen(pen);
                _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice])->setData(X, *_listData[numDevice]);
                GraphProcessingV2(numDevice,currentIndexGraph[numDevice], _listData [numDevice], 10);
                currentIndexGraph[numDevice]++;
                if(currentIndexGraph[numDevice]==(maxGraphOnScene-1))
                    currentIndexGraph[numDevice]=0;
                _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice])->data().clear();
                _listData [numDevice]->clear();
                pen.setColor(QColor(255, 0, 0));
                 pen.setWidth(1);
                _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice])->setPen(pen);
            }
        }

    }
    else if(flagRadButtons[numDevice] == sRadioADC2){
        for(int i=0; i < listDevice->ADC2.size(); i++){

            if(listShift_[numDevice]!=0)
                listShift_[numDevice]--;
            else
                _listData [numDevice]->append(listDevice->ADC2.at(i));

            if(beginGraph[numDevice] != true){
                 _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice])->addData(X, *_listData[numDevice]);
                 _listGraph.at(numDevice)->rescaleAxes();      // Масштабируем график по данным
                 _listGraph.at(numDevice)->replot();           // Отрисовываем график
            }

            if(_listData [numDevice]->size() >= WidthGraph){
                pen.setColor(QColor(0, 0, 100));
                 pen.setWidth(1);
                _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice])->setPen(pen);
                _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice])->setData(X, *_listData[numDevice]);
                GraphProcessingV2(numDevice,currentIndexGraph[numDevice], _listData [numDevice], 10);
                currentIndexGraph[numDevice]++;
                if(currentIndexGraph[numDevice]==(maxGraphOnScene-1))
                    currentIndexGraph[numDevice]=0;
                _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice])->data().clear();
                _listData [numDevice]->clear();
                pen.setColor(QColor(255, 0, 0));
                 pen.setWidth(1);
                _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice])->setPen(pen);
            }
        }
    }
    else if(flagRadButtons[numDevice] == sRadioAccX){
        for(int i=0; i < listDevice->accX.size(); i++){
            if(listShift_[numDevice]!=0)
                listShift_[numDevice]--;
            else
                _listData [numDevice]->append(listDevice->accX.at(i));

            if(beginGraph[numDevice] != true){
                 _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice])->addData(X, *_listData[numDevice]);
                 _listGraph.at(numDevice)->rescaleAxes();      // Масштабируем график по данным
                 _listGraph.at(numDevice)->replot();           // Отрисовываем график
            }

            if(_listData [numDevice]->size() >= WidthGraph){
                pen.setColor(QColor(0, 0, 100));
                 pen.setWidth(1);
                _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice])->setPen(pen);
                _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice])->setData(X, *_listData[numDevice]);
                GraphProcessingV2(numDevice,currentIndexGraph[numDevice], _listData [numDevice], 10);
                currentIndexGraph[numDevice]++;
                if(currentIndexGraph[numDevice]==(maxGraphOnScene-1))
                    currentIndexGraph[numDevice]=0;
                _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice])->data().clear();
                _listData [numDevice]->clear();
                pen.setColor(QColor(255, 0, 0));
                 pen.setWidth(1);
                _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice])->setPen(pen);
            }
        }

    }
    else if(flagRadButtons[numDevice] == sRadioAccY){
        for(int i=0; i < listDevice->accY.size(); i++){
            if(listShift_[numDevice]!=0)
                listShift_[numDevice]--;
            else
                _listData [numDevice]->append(listDevice->accY.at(i));

            if(beginGraph[numDevice] != true){
                _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice])->addData(X, *_listData[numDevice]);
                _listGraph.at(numDevice)->rescaleAxes();      // Масштабируем график по данным
                _listGraph.at(numDevice)->replot();           // Отрисовываем график
            }
            if(_listData [numDevice]->size() >= WidthGraph){
                pen.setWidth(1);
                pen.setColor(QColor(0, 0, 100));
                _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice])->setPen(pen);
                _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice])->setData(X, *_listData[numDevice]);
                GraphProcessingV2(numDevice,currentIndexGraph[numDevice], _listData [numDevice], 10);
                currentIndexGraph[numDevice]++;
                if(currentIndexGraph[numDevice]==(maxGraphOnScene-1))
                    currentIndexGraph[numDevice]=0;
                _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice])->data().clear();
                _listData [numDevice]->clear();
                pen.setColor(QColor(255, 0, 0));
                pen.setWidth(1);
                _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice])->setPen(pen);
            }
        }
    }
    else if(flagRadButtons[numDevice] == sRadioAccZ){
        for(int i=0; i < listDevice->accZ.size(); i++){
            if(listShift_[numDevice]!=0)
                listShift_[numDevice]--;
            else
                _listData [numDevice]->append(listDevice->accZ.at(i));

            if(beginGraph[numDevice] != true){
                 _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice]+1)->addData(X, *_listData[numDevice]);
                 _listGraph.at(numDevice)->rescaleAxes();      // Масштабируем график по данным
                 _listGraph.at(numDevice)->replot();           // Отрисовываем график
            }

            if(_listData [numDevice]->size() >= WidthGraph){
                pen.setColor(QColor(0, 0, 100));
                 pen.setWidth(1);
                _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice])->setPen(pen);
                _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice])->setData(X, *_listData[numDevice]);
                GraphProcessingV2(numDevice,currentIndexGraph[numDevice], _listData [numDevice], 10);
                currentIndexGraph[numDevice]++;
                if(currentIndexGraph[numDevice]==(maxGraphOnScene-1))
                    currentIndexGraph[numDevice]=0;
                _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice])->data().clear();
                _listData [numDevice]->clear();
                 pen.setWidth(1);
                pen.setColor(QColor(255, 0, 0));
                _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice])->setPen(pen);
            }
        }

    }
    else if(flagRadButtons[numDevice] == sRadioTsens){
        for(int i=0; i < listDevice->Tsens.size(); i++){
            if(listShift_[numDevice]!=0)
                listShift_[numDevice]--;
            else
                _listData [numDevice]->append(listDevice->Tsens.at(i));

            if(beginGraph[numDevice] != true){
                 _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice])->addData(X, *_listData[numDevice]);
                 _listGraph.at(numDevice)->rescaleAxes();      // Масштабируем график по данным
                 _listGraph.at(numDevice)->replot();           // Отрисовываем график
            }

            if(_listData [numDevice]->size() >= WidthGraph){
                pen.setColor(QColor(0, 0, 100));
                 pen.setWidth(1);
                _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice])->setPen(pen);
                _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice])->setData(X, *_listData[numDevice]);
                GraphProcessingV2(numDevice,currentIndexGraph[numDevice], _listData [numDevice], 10);
                currentIndexGraph[numDevice]++;
                if(currentIndexGraph[numDevice]==(maxGraphOnScene-1))
                    currentIndexGraph[numDevice]=0;
                _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice])->data().clear();
                _listData [numDevice]->clear();
                pen.setColor(QColor(255, 0, 0));
                 pen.setWidth(1);
                _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice])->setPen(pen);
            }
        }
    }

    _listGraph.at(numDevice)->rescaleAxes();      // Масштабируем график по данным
    _listGraph.at(numDevice)->replot();           // Отрисовываем график
}

//void MainWindow::dataProccesing(pointsDevices *listDevice)
//{
//    numDevice = listDevice->numDevices;
//    /*заполняем _list данными в зависимости от выбранного radioButton*/
//    if(flagRadButtons[numDevice] == sRadioADC1){
//        for(int i=0; i < listDevice->ADC1.size(); i++){

//            if(listShift_[numDevice]!=0)
//                listShift_[numDevice]--;

//            else{
//                _listData [numDevice]->append(listDevice->ADC1.at(i));
//            }

//            if(beginGraph[numDevice] != true){
//                 _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice]+1)->addData(X, *_listData[numDevice]);
//                 _listGraph.at(numDevice)->rescaleAxes();      // Масштабируем график по данным
//                 _listGraph.at(numDevice)->replot();           // Отрисовываем график
//            }

//            if(_listData [numDevice]->size() >= WidthGraph){
//                //beginGraph[numDevice] = true;
//                _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice]+1)->data()->clear();
//                GraphProcessing(numDevice, _listData [numDevice], 1000);
//                _listData [numDevice]->clear();
//            }
//        }

//    }
//    else if(flagRadButtons[numDevice] == sRadioADC2){
//        for(int i=0; i < listDevice->ADC2.size(); i++){

//            if(listShift_[numDevice]!=0)
//                listShift_[numDevice]--;
//            else
//                _listData [numDevice]->append(listDevice->ADC2.at(i));

//            if(beginGraph[numDevice] != true){
//                 _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice]+1)->addData(X, *_listData[numDevice]);
//                 _listGraph.at(numDevice)->rescaleAxes();      // Масштабируем график по данным
//                 _listGraph.at(numDevice)->replot();           // Отрисовываем график
//            }

//            if(_listData [numDevice]->size() >= WidthGraph){
//                //beginGraph[numDevice] = true;
//                _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice]+1)->data()->clear();
//                GraphProcessing(numDevice, _listData [numDevice], 1000);
//                _listData [numDevice]->clear();
//            }



//        }

//    }
//    else if(flagRadButtons[numDevice] == sRadioAccX){
//        for(int i=0; i < listDevice->accX.size(); i++){
//            if(listShift_[numDevice]!=0)
//                listShift_[numDevice]--;
//            else
//                _listData [numDevice]->append(listDevice->accX.at(i));

//            if(beginGraph[numDevice] != true){
//                 _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice]+1)->addData(X, *_listData[numDevice]);
//                 _listGraph.at(numDevice)->rescaleAxes();      // Масштабируем график по данным
//                 _listGraph.at(numDevice)->replot();           // Отрисовываем график
//            }

//            if(_listData [numDevice]->size() >= WidthGraph){
//                //beginGraph[numDevice] = true;
//                _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice]+1)->data()->clear();
//                GraphProcessing(numDevice, _listData [numDevice], 1000);
//                _listData [numDevice]->clear();
//            }
//        }

//    }
//    else if(flagRadButtons[numDevice] == sRadioAccY){
//        for(int i=0; i < listDevice->accY.size(); i++){
//            if(listShift_[numDevice]!=0)
//                listShift_[numDevice]--;
//            else
//                _listData [numDevice]->append(listDevice->accY.at(i));

//            if(_listData [numDevice]->size() >= WidthGraph){
//                beginGraph[numDevice] = true;
//                GraphProcessing(numDevice, _listData [numDevice], 1000);
//                _listData [numDevice]->clear();
//            }
//        }
//        if(beginGraph[numDevice] != true){
//             _listGraph.at(numDevice)->graph(0)->addData(X, *_listData[numDevice]);
//        }
//    }
//    else if(flagRadButtons[numDevice] == sRadioAccZ){
//        for(int i=0; i < listDevice->accZ.size(); i++){
//            if(listShift_[numDevice]!=0)
//                listShift_[numDevice]--;
//            else
//                _listData [numDevice]->append(listDevice->accZ.at(i));

//            if(beginGraph[numDevice] != true){
//                 _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice]+1)->addData(X, *_listData[numDevice]);
//                 _listGraph.at(numDevice)->rescaleAxes();      // Масштабируем график по данным
//                 _listGraph.at(numDevice)->replot();           // Отрисовываем график
//            }

//            if(_listData [numDevice]->size() >= WidthGraph){
//                //beginGraph[numDevice] = true;
//                _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice]+1)->data()->clear();
//                GraphProcessing(numDevice, _listData [numDevice], 1000);
//                _listData [numDevice]->clear();
//            }
//        }

//    }
//    else if(flagRadButtons[numDevice] == sRadioTsens){
//        for(int i=0; i < listDevice->Tsens.size(); i++){
//            if(listShift_[numDevice]!=0)
//                listShift_[numDevice]--;
//            else
//                _listData[numDevice]->append(listDevice->Tsens.at(i));

//            if(beginGraph[numDevice] != true){
//                 _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice]+1)->addData(X, *_listData[numDevice]);
//                 _listGraph.at(numDevice)->rescaleAxes();      // Масштабируем график по данным
//                 _listGraph.at(numDevice)->replot();           // Отрисовываем график
//            }

//            if(_listData [numDevice]->size() >= WidthGraph){
//                //beginGraph[numDevice] = true;
//                _listGraph.at(numDevice)->graph(currentIndexGraph[numDevice]+1)->data()->clear();
//                GraphProcessing(numDevice, _listData [numDevice], 1000);
//                _listData [numDevice]->clear();
//            }
//        }

//    }

//    _listGraph.at(numDevice)->rescaleAxes();      // Масштабируем график по данным
//    _listGraph.at(numDevice)->replot();           // Отрисовываем график
//}
void MainWindow::graphicSettings()
{
    if (!file_global.open(QFile::ReadOnly|QFile::Text))
    {
        qDebug() << "Ошибка при открытии файла";
        return;
    }

    fileUI->show();
}

void MainWindow::openFile()
{
    QString path_file = "D:/test_server";
    QString file_name = QFileDialog::getOpenFileName(this, "Открыть файл с измерениями",
                                                     path_file, "Txt File(*.txt)");
    if(file_global.isOpen())
        file_global.close();
    file_global.setFileName(file_name);
    if (!file_global.open(QFile::ReadOnly|QFile::Text))
    {
        qDebug() << "Ошибка при открытии файла";
        return;
    }

    fileUI->show();
}

void MainWindow::getSettingMeasure(pointsMeasure_ *settingPoint)
{
    QVector<double> X;
    bool ok;
    uint8_t dev_num = 0;
    int16_t mes = 0;
    QString tmp;
    uint array_graphCount[6];
    uint32_t array_graphTmpBegin[6];
    QList<QVector<double>*> listMeasure;
    for(int i=0; i <6; i++){
        listMeasure.append(new QVector<double>);
        array_graphCount[i] = settingPoint->graphCounter;
        array_graphTmpBegin[i] = 0;
        for(uint cntGraph=0; cntGraph < (maxGraphOnScene+maxColorGraphics_); cntGraph++)
            _listGraph.at(i)->graph(cntGraph)->data()->clear();
        for(uint cnt_list_graph_color=0; cnt_list_graph_color < maxColorGraphics_; cnt_list_graph_color++)
            listColorGraph[i].at(cnt_list_graph_color)->mapValueGraph.clear();
    }
    /* Тут отчистить предыдущие измерения */
    QFile f_adc1, f_adc2, f_accX, f_accY, f_accZ, f_Tsens;
    f_adc1.setFileName("ADC1.txt");
    f_adc1.open(QIODevice::WriteOnly|QIODevice::Text);
    QTextStream streamADC1( &f_adc1 );

    f_adc2.setFileName("ADC2.txt");
    f_adc2.open(QIODevice::WriteOnly|QIODevice::Text);
    QTextStream streamADC2( &f_adc2 );

    f_accX.setFileName("AccX.txt");
    f_accX.open(QIODevice::WriteOnly|QIODevice::Text);
    QTextStream streamAccX( &f_accX );

    f_accY.setFileName("AccY.txt");
    f_accY.open(QIODevice::WriteOnly|QIODevice::Text);
    QTextStream streamAccY( &f_accY );

    f_accZ.setFileName("AccZ.txt");
    f_accZ.open(QIODevice::WriteOnly|QIODevice::Text);
    QTextStream streamAccZ( &f_accZ );

    f_Tsens.setFileName("Tsens.txt");
    f_Tsens.open(QIODevice::WriteOnly|QIODevice::Text);
    QTextStream streamTsens (&f_Tsens );


    //X.resize(settingPoint->graphCounter);
    for (uint i = settingPoint->begin; i<(settingPoint->graphCounter + settingPoint->begin) ; i++)
        X.append(i);

    while(!file_global.atEnd()){
        QString text = file_global.read(32);
        if(text.startsWith("ed00ff", Qt::CaseInsensitive)){
            dev_num = text.midRef(6, 2).toUInt(&ok, 16) -1;
            if(array_graphCount[dev_num] == 0)
                continue;
            tmp = text.mid(8+0*4, 4); /*ADC 1*/
            mes = (tmp.left(2).toUInt(&ok, 16)) |( tmp.right(2).toUInt(&ok, 16) << 8);
            if(mes & 0x1000)
                mes |= 0xE000;
            streamADC1 << mes << '\n';

            tmp = text.mid(8+1*4, 4); /*ADC 2*/
            mes = (tmp.left(2).toUInt(&ok, 16)) |( tmp.right(2).toUInt(&ok, 16) << 8);
            if(mes & 0x1000)
                mes |= 0xE000;
            streamADC2 << QString::number(mes, 10) << '\n';

            tmp = text.mid(8+2*4, 4); /*AccX*/
            mes = (tmp.left(2).toUInt(&ok, 16)) |( tmp.right(2).toUInt(&ok, 16) << 8);
            streamAccX << QString::number(mes, 10) << '\n';

            tmp = text.mid(8+3*4, 4); /*AccY*/
            mes = (tmp.left(2).toUInt(&ok, 16)) |( tmp.right(2).toUInt(&ok, 16) << 8);
            streamAccY << QString::number(mes, 10) << '\n';

            tmp = text.mid(8+4*4, 4); /*AccZ*/
            mes = (tmp.left(2).toUInt(&ok, 16)) |( tmp.right(2).toUInt(&ok, 16) << 8);
            streamAccZ << QString::number(mes, 10) << '\n';

            tmp = text.mid(8+5*4, 4); /*Tsens*/
            mes = (tmp.left(2).toUInt(&ok, 16)) |( tmp.right(2).toUInt(&ok, 16) << 8);
            streamTsens << QString::number(mes, 10) << '\n';
        }
    }
   f_adc1.flush();
   f_adc1.close();

   f_adc2.flush();
   f_adc2.close();

   f_accX.flush();
   f_accX.close();

   f_accY.flush();
   f_accY.close();

   f_accZ.flush();
   f_accZ.close();

   f_Tsens.flush();
   f_Tsens.close();

//    while(!file_global.atEnd()){
//        QString text = file_global.read(32);
//        if(text.startsWith("ed00ff", Qt::CaseInsensitive)){
//            dev_num = text.midRef(6, 2).toUInt(&ok, 16) -1;
//            if(array_graphCount[dev_num] == 0)
//                continue;
//            tmp = text.mid(8+settingPoint->modX.at(dev_num)*4, 4); //8 - шапка, 4*.. сдвиг необходимого измерения
//            mes = (tmp.left(2).toUInt(&ok, 16)) |( tmp.right(2).toUInt(&ok, 16) << 8);

//            if(array_graphTmpBegin[dev_num] < settingPoint->begin){
//                array_graphTmpBegin[dev_num]++;
//                continue;
//            }

//            switch(settingPoint->modX.at(dev_num)){
//            case 0: //ADC1
//                if(mes & 0x1000)
//                    mes |= 0xE000;
//                qDebug() << "tyt";
//                break;
//            case 1: //ADC2
//                if(mes & 0x1000)
//                    mes |= 0xE000;
//                break;
//            case 2: //AccX
////                if(mes & 0x800)
////                    mes |= 0xF000;
//                break;
//            case 3: //AccY
////                if(mes & 0x800)
////                    mes |= 0xF000;
//                break;
//            case 4: //AccZ
////                if(mes & 0x800)
////                    mes |= 0xF000;
//                break;
//            case 5: //Tsense
//                if(mes & 0x1000)
//                    mes |= 0xE000;
//                break;

//            }

//            listMeasure.at(dev_num)->append(mes);
//            array_graphCount[dev_num]--;
//        }
//    }
    file_global.close();
//    pen.setWidth(1);
//    pen.setColor(QColor(0,0,100));

//    for(int i=0; i <6; i++){
//        _listGraph.at(i)->graph(0)->setPen(pen);
//    }
//    /* Построить сразу эти графики */
//    for(int i=0; i <6; i++){
//        _listGraph.at(i)->graph(0)->setData(X, *listMeasure.at(i));
//        _listGraph.at(i)->rescaleAxes();
//        _listGraph.at(i)->xAxis->setRange(settingPoint->begin, settingPoint->graphCounter + settingPoint->begin);
//        _listGraph.at(i)->replot();
//    }
}


void MainWindow::settingsSlot()
{
    m_cmdESP = SETTINGS_ESP;
    g_ = ui->lineG->text().toInt();
    hr_ = ui->lineHR->text().toInt();
    if(g_<2 || g_ > 8  || (g_ % 2)){

        QMessageBox messageBox; messageBox.critical(0,"Ошибка", "G должно быть: 2, 4 , 6, 8");
        return;
    }
    if(hr_<0 || hr_ > 1){
        QMessageBox messageBox; messageBox.critical(0,"Ошибка", "HR должно быть: 0 или 1");
        return;
    }
    emit cmdEspALL(m_cmdESP, g_, hr_ );
}

void MainWindow::startSlot()
{
    m_cmdESP = START_ESP;

    WidthGraph = ui->lineWidth->text().toUInt();
    shiftGraph = ui->lineShift->text().toUInt();
    initListMeasure();
    ui->lineShift->setEnabled(false);
    ui->lineWidth->setEnabled(false);
    ui->pbChangeShift->setEnabled(false);
    connect(_server, SIGNAL(newData(pointsDevices*)), this, SLOT(dataProccesing(pointsDevices*)));
    emit cmdEspALL(m_cmdESP, g_, hr_ );
}
void MainWindow::stopSlot()
{
    m_cmdESP = STOP_ESP;
    ui->lineShift->setEnabled(true);
    ui->lineWidth->setEnabled(true);
    ui->pbChangeShift->setEnabled(true);

    disconnect(_server, SIGNAL(newData(pointsDevices*)), this, SLOT(dataProccesing(pointsDevices*)));
    emit cmdEspALL(m_cmdESP, g_, hr_ );
}

void MainWindow::searchSlot()
{

}

void MainWindow::initGraphics()
{

    /* Set width Pen and set Color */
    pen.setWidth(1);
    pen.setColor(QColor(255, 0, 0));

    for(int i=0; i<_devNumber; i++){
        for(int cntGraph=0; cntGraph < maxGraphOnScene; cntGraph++){
            _listGraph.at(i)->addGraph(); // blue line
            _listGraph.at(i)->graph(cntGraph)->setPen(pen);
        }

        mColorMap1 = new QCPColorMap(_listGraph.at(i)->xAxis, _listGraph.at(i)->yAxis2);
        //mColorMap1->data()->setRange(QCPRange(0, 50), QCPRange(0, 1));
        mColorMap1->data()->setSize(0, 1);
        mColorMap1->setInterpolate(0);
        mColorScale1 = new QCPColorScale(_listGraph.at(i));
        //mColorScale1->rescaleDataRange(true);
        _listGraph.at(i)->plotLayout()->addElement(0, 1, mColorScale1);
        mColorMap1->setColorScale(mColorScale1);

        QCPColorGradient *mGradient = new QCPColorGradient();
        mColorMap1->setGradient(mGradient->gpJet );
        mGradient->setLevelCount(20);
        mColorMap1->setDataRange(QCPRange(1, maxColorGraphics_));   //range тепловой карты
        //mColorMap1->rescaleDataRange(true);
        _listGraph.at(i)->rescaleAxes();
        _listGraph.at(i)->replot();

//    QFont legendFont = font();
//    legendFont.setPointSize(8);
//        _listGraph.at(i)->legend->setVisible(true);
//        _listGraph.at(i)->legend->setFont(legendFont);
//        _listGraph.at(i)->legend->setSelectedFont(legendFont);
//        _listGraph.at(i)->legend->setIconBorderPen(Qt::NoPen);
//        _listGraph.at(i)->legend->setSelectableParts(QCPLegend::spNone);
//        _listGraph.at(i)->legend->setIconSize(9, 9);
//        _listGraph.at(i)->graph()->setName(QString("Модуль %1").arg(i+1));
    }
}

//void MainWindow::ReinitGraphic(int numDev)
//{
//    _listGraph.at(numDev)->clearGraphs();

//    _listGraph.at(numDev)->addGraph(); // blue line
//    _listGraph.at(numDev)->graph(0)->setPen(QPen(QColor(0, 0, 100)));

//    mColorMap1 = new QCPColorMap(_listGraph.at(numDev)->xAxis, _listGraph.at(numDev)->yAxis);
//    mColorMap1->rescaleDataRange(true);
//    //mColorMap1->data()->setRange(QCPRange(0, 50), QCPRange(0, 1));
//    mColorMap1->data()->setSize(0, 5);
//    mColorMap1->setInterpolate(0);
//    mColorScale1 = new QCPColorScale(_listGraph.at(numDev));
//    _listGraph.at(numDev)->plotLayout()->addElement(0, 1, mColorScale1);
//    mColorMap1->setColorScale(mColorScale1);

//    QCPColorGradient *mGradient = new QCPColorGradient();
//    mColorMap1->setGradient(mGradient->gpJet );
//    mGradient->setLevelCount(20);
//    mColorMap1->setDataRange(QCPRange(0, maxColorGraphics_));   //range тепловой карты

//    _listGraph.at(numDev)->rescaleAxes();
//    _listGraph.at(numDev)->replot();
//}

void MainWindow::initListMeasure()
{
    /* если проинициализировано уже */
    if(flagRadButtons.count()){
        flagRadButtons.clear();
        X.clear();
        for(int devNum=0; devNum < _devNumber; devNum++){
            //delete vecCntGraph_.first();
            //delete _listData.first();

//            for(int i=0; i <maxGraphOnScene; i++ ){
//                _listGraph[devNum]->graph(i)->data().clear();
//            }
//             _listGraph[devNum]->replot();
        }
        _listData.clear();  //Утечка памяти
        vecCntGraph_.clear();
    }

    for(int i=0; i < _devNumber; i++)
    {
        listShift_[i] = shiftGraph;
        /* GOTO: Считывать текущее значение*/
        flagRadButtons.append(getTextRadBut(i));
       _listData.append((new QVector <double>[WidthGraph]));

       vecCntGraph_.append(new QVector <uint8_t>);
       vecCntGraph_[i]->resize(maxGraphOnScene+1);
       currentIndexGraph[i] = 0;
       beginGraph[i] = false;
    }
    for(int i=0; i < WidthGraph; i++) X.append(i);
}

void MainWindow::initColorGraphs()
{
    pen.setWidth(4);

    for (int cnt_cstPlot=0; cnt_cstPlot <  _listGraph.size(); cnt_cstPlot++){
        listColorGraph.append(QList <graphColor*>());
        for(uint32_t cnt = 0; cnt < maxColorGraphics_; cnt++){
            listColorGraph[cnt_cstPlot].append(new graphColor());
            listColorGraph[cnt_cstPlot].at(cnt)->color = wColorScale->getColor((double)cnt/maxColorGraphics_);
            listColorGraph[cnt_cstPlot].at(cnt)->graph = _listGraph.at(cnt_cstPlot)->addGraph();
            pen.setColor(listColorGraph[cnt_cstPlot].at(cnt)->color);
            listColorGraph[cnt_cstPlot].at(cnt)->graph->setPen(pen);
            listColorGraph[cnt_cstPlot].at(cnt)->graph->setLineStyle(QCPGraph::lsNone);              //точки не соединять
            listColorGraph[cnt_cstPlot].at(cnt)->graph->setScatterStyle(QCPScatterStyle::ssDot);  //круглая точка
        }
    }
}

void MainWindow::deInitColorGraphs()
{
    for (int cnt_cstPlot=0; cnt_cstPlot <  _listGraph.size(); cnt_cstPlot++){

        for(uint cntGraph=0; cntGraph < (maxGraphOnScene+maxColorGraphics_); cntGraph++)
            _listGraph.at(cnt_cstPlot)->graph(cntGraph)->data()->clear();
        for(uint cnt_list_graph_color=0; cnt_list_graph_color < maxColorGraphics_; cnt_list_graph_color++)
            listColorGraph[cnt_cstPlot].at(cnt_list_graph_color)->mapValueGraph.clear();

        for(uint32_t cnt = 0; cnt < maxColorGraphics_; cnt++){
            listColorGraph[cnt_cstPlot].append(new graphColor());
            delete listColorGraph[cnt_cstPlot].at(cnt);
        }
    }
    listColorGraph.clear();

}

QString MainWindow::getTextRadBut(int id)
{
    int numButton = butGrList.at(id)->checkedId();

    switch(numButton){
    case 0:

        return sRadioADC1;
        break;

    case 1:
        return sRadioADC2;
        break;

    case 2:
        return sRadioAccX;

        break;

    case 3:
        return sRadioAccY;

        break;

    case 4:
        return sRadioAccZ;

        break;

    case 5:
        return sRadioTsens;

        break;
    }
    return "abcg";
}



void MainWindow::test_plot()
{

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

    butGrList.append(ui->buttonGroup);
    butGrList.append(ui->buttonGroup_2);
    butGrList.append(ui->buttonGroup_3);
    butGrList.append(ui->buttonGroup_4);
    butGrList.append(ui->buttonGroup_5);
    butGrList.append(ui->buttonGroup_6);
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
        ui->lineShift->setEnabled(true);
        ui->lineWidth->setEnabled(true);
        ui->pbChangeShift->setEnabled(true);
    }
    else if(cmd == "SETTINGS"){
        m_cmdESP = SETTINGS_ESP;
        g_ = ui->lineG->text().toInt();
        hr_ = ui->lineHR->text().toInt();
        if(g_<2 || g_ > 8){
            QMessageBox messageBox; messageBox.critical(0,"Ошибка", "G должно быть: 2-8");
            return;
        }
        if(hr_< 0 || hr_ > 1){
            QMessageBox messageBox; messageBox.critical(0,"Ошибка", "Шаг должно быть: 2-10");
            return;
        }

    }
    else if(cmd == "SEARCH DEVICE"){
        m_cmdESP = SEARCH_ESP;
    }
    emit cmdEspALL(m_cmdESP, g_, hr_ );


}

void MainWindow::changeStrRadBut(int idGroup, int numButton)
{
    _listData[idGroup]->clear();
    listShift_[idGroup] = shiftGraph;
    vecCntGraph_[idGroup]->clear();
    vecCntGraph_[idGroup]->resize(maxGraphOnScene+1);
    currentIndexGraph[idGroup] = 0;

    beginGraph[idGroup] = false;

    switch(numButton){
    case 0:
        flagRadButtons[idGroup] = sRadioADC1;
        _listGraph.at(idGroup)->yAxis->setLabel( "ADC1" );
        for(uint cntGraph=0; cntGraph < (maxGraphOnScene+maxColorGraphics_); cntGraph++)
            _listGraph.at(idGroup)->graph(cntGraph)->data().data()->clear();
        for(uint cnt_list_graph_color=0; cnt_list_graph_color < maxColorGraphics_; cnt_list_graph_color++)
            listColorGraph[idGroup].at(cnt_list_graph_color)->mapValueGraph.clear();
        pen.setColor(QColor(255, 0, 0));
        pen.setWidth(1);
        _listGraph.at(idGroup)->graph(0)->setPen(pen);

        break;

    case 1:
        flagRadButtons[idGroup] = sRadioADC2;
        _listGraph.at(idGroup)->yAxis->setLabel( "ADC2" );
        for(uint cntGraph=0; cntGraph < (maxGraphOnScene+maxColorGraphics_); cntGraph++)
            _listGraph.at(idGroup)->graph(cntGraph)->data().data()->clear();
        for(uint cnt_list_graph_color=0; cnt_list_graph_color < maxColorGraphics_; cnt_list_graph_color++)
            listColorGraph[idGroup].at(cnt_list_graph_color)->mapValueGraph.clear();
        pen.setColor(QColor(255, 0, 0));
        pen.setWidth(1);
        _listGraph.at(idGroup)->graph(0)->setPen(pen);
        break;

    case 2:
        flagRadButtons[idGroup] = sRadioAccX;
        _listGraph.at(idGroup)->yAxis->setLabel( "AccX" );
        for(uint cntGraph=0; cntGraph < (maxGraphOnScene+maxColorGraphics_); cntGraph++)
            _listGraph.at(idGroup)->graph(cntGraph)->data().data()->clear();
        for(uint cnt_list_graph_color=0; cnt_list_graph_color < maxColorGraphics_; cnt_list_graph_color++)
            listColorGraph[idGroup].at(cnt_list_graph_color)->mapValueGraph.clear();
        pen.setColor(QColor(255, 0, 0));
        pen.setWidth(1);
        _listGraph.at(idGroup)->graph(0)->setPen(pen);
        break;

    case 3:
        flagRadButtons[idGroup] = sRadioAccY;
        _listGraph.at(idGroup)->yAxis->setLabel( "AccY" );
        for(uint cntGraph=0; cntGraph < (maxGraphOnScene+maxColorGraphics_); cntGraph++)
            _listGraph.at(idGroup)->graph(cntGraph)->data().data()->clear();
        for(uint cnt_list_graph_color=0; cnt_list_graph_color < maxColorGraphics_; cnt_list_graph_color++)
            listColorGraph[idGroup].at(cnt_list_graph_color)->mapValueGraph.clear();
        pen.setColor(QColor(255, 0, 0));
        pen.setWidth(1);
        _listGraph.at(idGroup)->graph(0)->setPen(pen);

        break;

    case 4:
        flagRadButtons[idGroup] = sRadioAccZ;
        _listGraph.at(idGroup)->yAxis->setLabel( "AccZ" );
        for(uint cntGraph=0; cntGraph < (maxGraphOnScene+maxColorGraphics_); cntGraph++)
            _listGraph.at(idGroup)->graph(cntGraph)->data().data()->clear();
        for(uint cnt_list_graph_color=0; cnt_list_graph_color < maxColorGraphics_; cnt_list_graph_color++)
            listColorGraph[idGroup].at(cnt_list_graph_color)->mapValueGraph.clear();
        pen.setColor(QColor(255, 0, 0));
        pen.setWidth(1);
        _listGraph.at(idGroup)->graph(0)->setPen(pen);
        break;

    case 5:
        flagRadButtons[idGroup] = sRadioTsens;
        _listGraph.at(idGroup)->yAxis->setLabel( "T" );
        for(uint cntGraph=0; cntGraph < (maxGraphOnScene+maxColorGraphics_); cntGraph++)
            _listGraph.at(idGroup)->graph(cntGraph)->data().data()->clear();
        for(uint cnt_list_graph_color=0; cnt_list_graph_color < maxColorGraphics_; cnt_list_graph_color++)
            listColorGraph[idGroup].at(cnt_list_graph_color)->mapValueGraph.clear();
        pen.setColor(QColor(255, 0, 0));
        pen.setWidth(1);
        _listGraph.at(idGroup)->graph(0)->setPen(pen);
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

void MainWindow::on_pbChangeShift_clicked()
{
    WidthGraph = ui->lineWidth->text().toUInt();
    shiftGraph = ui->lineShift->text().toUInt();

    for(uint idGroup=0; idGroup < 6; idGroup++){
        for(uint cntGraph=0; cntGraph < (maxGraphOnScene+maxColorGraphics_); cntGraph++)
            _listGraph.at(idGroup)->graph(cntGraph)->data().data()->clear();
        _listGraph.at(idGroup)->rescaleAxes();
        _listGraph.at(idGroup)->replot();
    }

    deInitColorGraphs();
    initColorGraphs();
    initListMeasure();
}

