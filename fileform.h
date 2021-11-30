#ifndef FILEFORM_H
#define FILEFORM_H

#include <QWidget>
#include <QList>
#include <QComboBox>
/* int mod_x
 * 0 - ADC1
 * 1 - ADC2
 * 2 - AccX
 * 3 - AccY
 * 4 - AccZ
 * 5 - Tsense
*/

typedef struct
{
    int32_t beginMeas;         //начало графика
    int32_t endMeas;  //количество точек
    QVector <uint8_t> modX;

} pointsMeasure_;

namespace Ui {
class fileForm;
}

class fileForm : public QWidget
{
    Q_OBJECT

public:
    explicit fileForm(QWidget *parent = nullptr);
    ~fileForm();
private:
    int cmBoxProccessing(QString);
private slots:
    void on_pushButton_clicked();
signals:
    void chooseMeasureModules(pointsMeasure_*);
private:
    Ui::fileForm *ui;
    QList<QComboBox*> listCmBox;
    pointsMeasure_ pointsMeasure;

};

#endif // FILEFORM_H
