#ifndef COLORSCALE_H
#define COLORSCALE_H

#include <QWidget>
#include <QPainter>
#include <QtDebug>
namespace Ui {
class ColorScale;
}

class ColorScale : public QWidget
{
    Q_OBJECT

public:
    explicit ColorScale(QWidget *parent = nullptr);
    ~ColorScale();
    QColor getColor(double value);

private:
    Ui::ColorScale *ui;
    QMap<double, QColor> mGradientColors;
    double x2, x1;
    QColor startPoint, endPoint;
protected:
    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event);
        QPainter painter(this);
        painter.setOpacity(0.9);
        painter.setRenderHint(QPainter::HighQualityAntialiasing);
        QLinearGradient gradient(0, 0, 1, height());
        QGradientStops stops;

        stops << QGradientStop(0, QColor(0, 0, 100));
        stops << QGradientStop(0.15, QColor(0, 50, 255));
        stops << QGradientStop(0.35, QColor(0, 255, 255));
        stops << QGradientStop(0.65, QColor(255, 255, 0));
        stops << QGradientStop(0.85, QColor(255, 30, 0));
        stops << QGradientStop(1, QColor(100, 0, 0));
        gradient.setStops(stops);

        painter.fillRect(rect(), gradient);
    }
};

#endif // COLORSCALE_H
