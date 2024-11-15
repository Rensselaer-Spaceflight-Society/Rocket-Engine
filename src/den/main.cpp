#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QThread>
#include <QtCharts/QSplineSeries>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QList>
#include "command.h"
#include "thre.h"
#include "recv.h"

QSerialPort *COMPORT = new QSerialPort();

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QTimer timer;
    QList <command> command_queue;

    // Example commands
    command terminate("Shutdown", 0);
    command ignition("ignition", 1);


    for (QSerialPortInfo &portInfo: QSerialPortInfo::availablePorts()){
        qDebug() << "Port Name : " << portInfo.portName();
        qDebug() << "Port Description : " << portInfo.description();
        qDebug() << "\n";
    }

    // Serial COMM Setup
    // bool retransmit_bool = false;

    // QSerialPort COMPORT;
    // COMPORT.setPortName("tty.URT1");
    // COMPORT.setBaudRate(QSerialPort::BaudRate::Baud9600);
    // COMPORT.setParity(QSerialPort::Parity::NoParity);
    // COMPORT.setFlowControl(QSerialPort::FlowControl::NoFlowControl);
    // COMPORT.setDataBits(QSerialPort::DataBits::Data8);
    // COMPORT.setStopBits(QSerialPort::StopBits::OneStop);
    // if(!COMPORT.open(QIODevice::ReadWrite)){
    //     qDebug() << "Failed to open serial port";
    //     return -1;
    // }

    // 
    QObject::connect(&w, &MainWindow::ignitionStart, [](){
        qDebug() << "ignition";
    });

    // At T-minus 10 seconds tell the rocket to begin inert flush
    QObject::connect(&w, &MainWindow::inertFlush, [](){
        qDebug() << "Inert flush";
    });

    // At T-minus 5 seconds tell the rocket to open the fuel pressure valve
    QObject::connect(&w, &MainWindow::presFuel, [](){
        qDebug() << "Open fuel pressurization valve";
    });

    // Insert ping into the command queue
    // QObject::connect(&timer, &QTimer::timeout, [&command_queue](){
    //     command ping("ping", 2);
    //     command_queue.append(ping);
    //     // qDebug() << "ping";
    // });
    // timer.start(200);
    // thre thread(&COMPORT, &retransmit_bool, &command_queue);
    // recv receive(&COMPORT, &retransmit_bool, &command_queue);

    // QObject::connect(&thread, &thre::sendFinished, &receive, &recv::startReceiving);
    // QObject::connect(&receive, &recv::receiveFinished, &thread, &thre::receiveFinished);

    // thread.start();

    return a.exec();
}
