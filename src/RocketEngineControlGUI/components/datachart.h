#ifndef DATACHART_H
#define DATACHART_H

#include <QWidget>
#include <QtCharts/QtCharts>

enum class ChartType {
    Force,
    Temperature,
    Pressure
};

namespace Ui {
class DataChart;
}

class DataChart : public QWidget
{
    Q_OBJECT

public:
    explicit DataChart(QWidget *parent = nullptr);
    ~DataChart();
    void append(double xValue, double yValue);
    void setChartType(ChartType chartType);
    void setChartTitle(const QString& title);

private:
    Ui::DataChart *ui;
    QChart * chart;
    QAreaSeries * series;
    QLineSeries * lowerSeries;
    QLineSeries * upperSeries;
    QValueAxis * axisX;
    QValueAxis * axisY;
    double maxYValue = 1;
};

#endif // DATACHART_H
