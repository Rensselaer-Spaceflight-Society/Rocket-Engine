#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QThread>
#include <QtCharts/QSplineSeries>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "thre.h"
#include "recv.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // QLineSeries *series = new QLineSeries();
    // series->append(0, 6);
    // series->append(2, 0);
    // series->append(3, 8);
    // series->append(7, 4);
    // series->append(10, 5);

    // QChart *chart = new QChart();
    // chart->legend()->hide();
    // chart->addSeries(series);
    // chart->createDefaultAxes();
    // chart->setTitle("Simple Line Chart");

    // QChartView *chartView = new QChartView(chart);
    // chartView->setRenderHint(QPainter::Antialiasing);

    // QMainWindow window;
    // window.setCentralWidget(chartView);
    // window.resize(800, 800);
    // window.show();

    // for (QSerialPortInfo &portInfo: QSerialPortInfo::availablePorts()){
    //     qDebug() << "Port Name : " << portInfo.portName();
    //     qDebug() << "Port Description : " << portInfo.description();
    // }

    // Serial COMM Setup
    bool retransmit_bool = false;

    QSerialPort COMPORT;
    COMPORT.setPortName("cu.URT1");
    COMPORT.setBaudRate(QSerialPort::BaudRate::Baud9600);
    COMPORT.setParity(QSerialPort::Parity::NoParity);
    COMPORT.setFlowControl(QSerialPort::FlowControl::NoFlowControl);
    COMPORT.setDataBits(QSerialPort::DataBits::Data8);
    COMPORT.setStopBits(QSerialPort::StopBits::OneStop);
    if(!COMPORT.open(QIODevice::ReadWrite)){
        qDebug() << "Failed to open serial port";
        return -1;
    }

    thre thread(&COMPORT, &retransmit_bool);
    recv receive(&COMPORT, &retransmit_bool);

    QObject::connect(&thread, &thre::sendFinished, &receive, &recv::startReceiving);
    QObject::connect(&receive, &recv::receiveFinished, &thread, &thre::receiveFinished);

    thread.start();
    
    return a.exec();
}
