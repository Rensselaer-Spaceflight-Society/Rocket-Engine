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

        int retryCount = 0;

        while (retryCount < maxRetries) {
            if (COMPORT->isOpen() && COMPORT->isRequestToSend() && i != command_queue->end()) {
                qDebug() << "Sending:" << i->getCommand();
                COMPORT->write(data);
                COMPORT->flush();
                COMPORT->waitForBytesWritten(2000);
    
                // Simulate receiving an acknowledgment
                if (COMPORT->waitForReadyRead(2000)) {
                    QByteArray receivedData = COMPORT->readAll();
                    qDebug() << "Received data:" << receivedData;
        
                    if (receivedData == data) command_queue->erase(i);
                } else qDebug() << "No acknowledgment received. Retransmitting...";
            else {
                qDebug() << "Error with serial port during send/receive.";
                break;
            }
            retryCount++
        }
        if (retryCount == maxRetries) {
            qDebug() << "Max retries reached. Command discarded.";
            command_queue->erase(i);
        }
        emit operationFinished();
    }    

private:
    QSerialPort* COMPORT;
    QList<command>* command_queue;
};

#endif // SERIAL_HANDLER_H
