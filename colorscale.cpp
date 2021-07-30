#include "colorscale.h"
#include "ui_colorscale.h"

ColorScale::ColorScale(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ColorScale)
{
    ui->setupUi(this);

    mGradientColors.insert(0.0 , QColor(0, 0, 100));
    mGradientColors.insert(0.15 , QColor(0, 50, 255));
    mGradientColors.insert(0.35, QColor(0, 255, 255));
    mGradientColors.insert(0.65,  QColor(255, 255, 0));
    mGradientColors.insert(0.85, QColor(255, 30, 0));
    mGradientColors.insert(1.0, QColor(100, 0, 0));

    getColor(0.69);
}

ColorScale::~ColorScale()
{
    delete ui;
}

QColor ColorScale::getColor(double value)
{
    if(0 <= value  && value < 0.15){
        x1 = 0;
        x2 = 0.15;
    }
    else if(0.15 <= value  && value < 0.35){
        x1 = 0.15;
        x2 = 0.35;
    }
    else if(0.35 <= value  && value < 0.65){
        x1 = 0.35;
        x2 = 0.65;
    }
    else if(0.65 <= value  && value < 0.85){
        x1 = 0.65;
        x2 = 0.85;
    }
    else if(0.85 <= value  && value < 1){
        x1 = 0.85;
        x2 = 1;
    }
    else{
        return QColor(100, 0, 0);
    }

    startPoint = mGradientColors.value(x2);
    endPoint = mGradientColors.value(x1);

    double segmentLength = (x2-x1);
    double pdist = (value-x1);
    double ratio = pdist/segmentLength;

    int r = (int)(ratio*startPoint.red() + (1-ratio)*endPoint.red());
    int g = (int)(ratio*startPoint.green() + (1-ratio)*endPoint.green());
    int b = (int)(ratio*startPoint.blue() + (1-ratio)*endPoint.blue());
    qDebug() << r << g << b;
    return QColor::fromRgb(r,g,b);;
}
