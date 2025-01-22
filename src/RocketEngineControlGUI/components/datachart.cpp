#include "datachart.h"
#include "ui_datachart.h"

DataChart::DataChart(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DataChart)
{
    ui->setupUi(this);
    this->chart = new QChart();
    this->series = new QAreaSeries();
    this->lowerSeries = new QLineSeries();
    this->upperSeries = new QLineSeries();
    this->axisX = new QValueAxis();
    this->axisY = new QValueAxis();

    series->setLowerSeries(lowerSeries);
    series->setUpperSeries(upperSeries);
    series->setName("Data");

    axisX->setLabelFormat("%.2f");
    axisX->setTickCount(5);
    axisX->setRange(0, 1);
    axisX->setTitleBrush(QColor(0xffefefef));
    axisX->setLabelsColor(QColor(0xffefefef));
    axisX->setTitleText("Time (s)");

    axisY->setLabelFormat("%.2f");
    axisY->setTickCount(5);
    axisY->setRange(0, 1);
    axisY->setTitleBrush(QColor(0xffefefef));
    axisY->setLabelsColor(QColor(0xffefefef));

    chart->addSeries(series);
    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    chart->setBackgroundBrush(QColor(0,0,0,0));
    chart->setTitleBrush(QColor(0xffefefef));
    chart->legend()->setVisible(false);

    ui->ChartView->setChart(chart);
    chart->setMargins(QMargins(0,4,0,0));
}

void DataChart::setChartTitle(const QString& title)
{
    chart->setTitle(title);
}

void DataChart::setChartType(ChartType chartType)
{
    if(chartType == ChartType::Force)
    {
        series->setColor(QColor(164, 215, 166, 64));
        series->setBorderColor(QColor(114, 147, 115));
        axisY->setTitleText("Force (N)");
    }
    else if(chartType == ChartType::Pressure)
    {
        series->setColor(QColor(101, 179, 242, 64));
        series->setBorderColor(QColor(73, 132, 178));
        axisY->setTitleText("Pressure (kPa)");
    }
    else
    {
        series->setColor(QColor(197, 73, 35, 64));
        series->setBorderColor(QColor(137, 46, 19));
        axisY->setTitleText("Temp (C)");
    }
}

void DataChart::append(float xValue, float yValue)
{
    lowerSeries->append(xValue, 0);
    upperSeries->append(xValue, yValue);
    maxXValue = std::fmax(maxXValue, xValue);
    minXValue = std::fmin(minXValue, xValue);
    maxYValue = std::fmax(maxYValue, yValue);
    axisX->setRange(minXValue, (int) maxXValue);
    axisY->setRange(0, (int) (maxYValue));
}

void DataChart::reset()
{
    minXValue = 0;
    maxXValue = 1;
    maxYValue = 1;
    lowerSeries->clear();
    upperSeries->clear();
    axisX->setRange(minXValue, maxXValue);
    axisY->setRange(0, maxYValue);
}

DataChart::~DataChart()
{
    delete axisX;
    delete axisY;
    delete lowerSeries;
    delete upperSeries;
    delete series;
    delete chart;
    delete ui;
}
