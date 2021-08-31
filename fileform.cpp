#include "fileform.h"
#include "ui_fileform.h"
#include "QDebug"

fileForm::fileForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::fileForm)
{
    ui->setupUi(this);
    listCmBox.append(ui->comboBox1);
    listCmBox.append(ui->comboBox2);
    listCmBox.append(ui->comboBox3);
    listCmBox.append(ui->comboBox4);
    listCmBox.append(ui->comboBox5);
    listCmBox.append(ui->comboBox6);

    for(int i=0; i < 6; i++)
        pointsMeasure.modX.append(0);   //Заполним нулями




}

fileForm::~fileForm()
{
    delete ui;
}

int fileForm::cmBoxProccessing(QString cmBoxText)
{
    if(cmBoxText == "ADC1")
        return 0;
    else if(cmBoxText == "ADC2")
        return 1;

    else if(cmBoxText == "AccX")
        return 2;

    else if(cmBoxText == "AccY")
        return 3;

    else if(cmBoxText == "AccZ")
        return 4;

    else
        return 5;

}

void fileForm::on_pushButton_clicked()
{
    QString text_cmBox;
    uint8_t tmp_num;
    for(int i=0; i<6; i++){
        text_cmBox = listCmBox.at(i)->currentText();
        tmp_num = cmBoxProccessing(text_cmBox);
        pointsMeasure.modX[i] = tmp_num;
    }
    pointsMeasure.begin = ui->lineEdBegin->text().toUInt();
    pointsMeasure.graphCounter = ui->lineEdCount->text().toUInt();
    emit chooseMeasureModules(&pointsMeasure);

    this->close();
}
