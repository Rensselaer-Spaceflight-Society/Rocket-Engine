#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QThread>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QList>
#include "serial_handler.h"
#include "command.h"

QSerialPort *COMPORT = new QSerialPort();

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QTimer timer;
    QList <command> command_queue;

    QSerialPort COMPORT;
    COMPORT.setPortName("tty.usbserial-A50285BI");
    COMPORT.setBaudRate(QSerialPort::BaudRate::Baud9600);
    COMPORT.setParity(QSerialPort::Parity::NoParity);
    COMPORT.setFlowControl(QSerialPort::FlowControl::NoFlowControl);
    COMPORT.setDataBits(QSerialPort::DataBits::Data8);
    COMPORT.setStopBits(QSerialPort::StopBits::OneStop);

    if (!COMPORT.open(QIODevice::ReadWrite)) {
        qDebug() << "Failed to open serial port";
        return -1;
    } 
    
    // Example commands
    command terminate("Shutdown", 0);
    command ignition("ignition", 1);

    command_queue.push_back(terminate);
    command_queue.push_back(ignition);

    bool retransmit_bool = false;
    SerialHandler serialHandler(&COMPORT, &retransmit_bool, &command_queue);

    QObject::connect(&serialHandler, &SerialHandler::operationFinished, [&serialHandler]() {
        qDebug() << "Operation complete. Starting next.";
        serialHandler.process();
    });

    serialHandler.process();

    return a.exec();
}
