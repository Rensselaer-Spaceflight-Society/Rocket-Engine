#ifndef THRE_H
#define THRE_H

#include <QThread>
#include <QSerialPort>
#include <QList>
#include <QDebug>
#include <QMutex>
#include "command.h"

class thre : public QThread
{
    Q_OBJECT
public:
    explicit thre(QSerialPort* port, bool* retrans, QList<command> *commands, QObject* parent = nullptr) : QThread(parent), COMPORT(port), command_queue(commands), retransmit_bool(retrans) {}

public slots:
    void receiveFinished(){
        start();
    }

signals:
    void sendFinished();


protected:
    void run() override{
        qDebug() << "Send Thread";
        auto i = command_queue->begin();
        QByteArray data = i->getCommand().toUtf8();         // Get the command

        if (COMPORT->isOpen() && COMPORT->isRequestToSend()) {
            COMPORT->write(data);
            COMPORT->flush();
            COMPORT->waitForBytesWritten(2000);
            emit sendFinished();  // Emit signal when finished
        } else {
            // qDebug() << "Error opening serial port for sending.";
        }
        qDebug() << "Send Thread done\n";
    }

private:
    QSerialPort *COMPORT;
    QList<command> *command_queue;
    bool retransmit_bool;
};

#endif // THRE_H
