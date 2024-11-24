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
        if (command_queue->isEmpty()) {
            qDebug() << "No commands in the queue";
            return;
        }

        auto i = command_queue->begin();
        QByteArray data = i->getCommand().toUtf8();

        if (COMPORT->isOpen() && COMPORT->isRequestToSend() && i != command_queue->end()) {
            qDebug() << "Sending:" << i->getCommand();
            COMPORT->write(data);
            COMPORT->flush();
            COMPORT->waitForBytesWritten(2000);
        }

        // Simulate receiving an acknowledgment
        if (COMPORT->waitForReadyRead(2000)) {
            QByteArray receivedData = COMPORT->readAll();
            qDebug() << "Received data:" << receivedData;

            if (receivedData == data) { // If acknowledgment matches
                command_queue->erase(i);
                *retransmit_bool = false;
            } else {
                *retransmit_bool = true;
            }
        } else {
            qDebug() << "No acknowledgment received. Retransmitting...";
            *retransmit_bool = true;
        }
    }    

private:
    QSerialPort* COMPORT;
    QList<command>* command_queue;
    bool* retransmit_bool;
};

#endif // SERIAL_HANDLER_H
