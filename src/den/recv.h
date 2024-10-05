#ifndef RECV_H
#define RECV_H

#include <QSerialPort>
#include <QThread>
#include <QMutex>
#include <QDebug>

class recv : public QThread
{
    Q_OBJECT

public:
    explicit recv(QSerialPort* port,bool* retrans, QObject* parent = nullptr) : QThread(parent), serial(port), retransmit_bool(retrans) {}

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
                QByteArray data = serial->readAll();
                qDebug() << "Data received:" << data;
                *retransmit_bool = false;
            }else {
                qDebug() << "Retransmission number: " << retransmit++;
                if(retransmit == 5){
                    qDebug() << "Retranmission attemps exceeded";
                    return;
                }
                *retransmit_bool = true;
            }

            emit receiveFinished();
        } else {
            qDebug() << "Error opening serial port for receiving.";
        }
        qDebug() << "Receive Thread done";
    }

private:
    QSerialPort *serial;
    int retransmit = 0;
    bool *retransmit_bool;
};

#endif // RECV_H
