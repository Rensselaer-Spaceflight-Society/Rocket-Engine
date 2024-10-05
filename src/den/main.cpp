#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QThread>
#include <QtCharts/QSplineSeries>


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QLineSeries *series = new QLineSeries();
    series->append(0, 6);
    series->append(2, 0);
    series->append(3, 8);
    series->append(7, 4);
    series->append(10, 5);

    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("Simple Line Chart");

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QMainWindow window;
    window.setCentralWidget(chartView);
    window.resize(800, 800);
    window.show();

    return a.exec();
}
