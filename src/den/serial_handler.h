#ifndef SERIAL_HANDLER_H
#define SERIAL_HANDLER_H

#include <QThread>
#include <QSerialPort>
#include <QList>
#include <QDebug>
#include "command.h"

class SerialHandler : public QThread
{
    Q_OBJECT
public:
    explicit SerialHandler(QSerialPort* port, bool* retrans, QList<command>* commands, QObject* parent = nullptr)
        : QThread(parent), COMPORT(port), command_queue(commands), retransmit_bool(retrans) {}

public slots:
    void process() {
        start();
    }

signals:
    void operationFinished();

protected:
    void run() override {
        

private:
    QSerialPort* COMPORT;
    QList<command>* command_queue;
    bool* retransmit_bool;
};

#endif // SERIAL_HANDLER_H
