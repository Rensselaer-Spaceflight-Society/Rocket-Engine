#ifndef RECV_H
#define RECV_H

#include <QSerialPort>
#include <QThread>
#include <QList>
#include <QMutex>
#include <QDebug>
#include "command.h"

class recv : public QThread
{
    Q_OBJECT

public:
    explicit recv(QSerialPort* port, bool* retrans, QList<command> *commands, QObject* parent = nullptr) : QThread(parent), serial(port), command_queue(commands), retransmit_bool(retrans) {}

public slots:
    void startReceiving() {
        start();  // Start the thread
    }

signals:
    void receiveFinished();

protected:
    void run() override {
        qDebug() << "Receive Thread";

        if (serial->isOpen()) {
            if (serial->waitForReadyRead(1000)) {
                auto i = command_queue->begin();
                QByteArray data = serial->read(8);

                // Check to see if it's an acknowledgement or data
                if(data == i->getCommand().toUtf8()){
                    command_queue->erase(i);                                // If it's an ack for the expected command, remove it from the list
                }else{
                    data += serial->read(33);                                // Otherwise its probably data, keep reading the rest of the data
                }

                // qDebug() << "Data received:" << data;
                *retransmit_bool = false;
            }else {
                // qDebug() << "Retransmission number: " << retransmit++;
                if(retransmit == 5){
                    qDebug() << "Retranmission attemps exceeded";
                    return;
                }
                *retransmit_bool = true;
            }

            emit receiveFinished();
        } else {
            // qDebug() << "Error opening serial port for receiving.";
        }
        qDebug() << "Receive Thread done";
    }

private:
    QSerialPort *serial;
    QList<command> *command_queue;
    int retransmit = 0;
    bool *retransmit_bool;
};

#endif // RECV_H
