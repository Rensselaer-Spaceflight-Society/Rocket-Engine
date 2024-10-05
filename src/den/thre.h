#ifndef THRE_H
#define THRE_H

#include <QThread>
#include <QSerialPort>
#include <QDebug>
#include <QMutex>

class thre : public QThread
{
    Q_OBJECT
public:
    explicit thre(QSerialPort* port,bool* retrans, QObject* parent = nullptr) : QThread(parent), COMPORT(port), retransmit_bool(retrans) {}

public slots:
    void receiveFinished(){
        start();
    }

signals:
    void sendFinished();


protected:
    void run() override{
        QByteArray data = "Hello from Sender";  // Data to send
        qDebug() << "Send Thread";

        if (COMPORT->isOpen() && COMPORT->isRequestToSend()) {
            COMPORT->write(data);
            COMPORT->flush();
            COMPORT->waitForBytesWritten(2000);
            emit sendFinished();  // Emit signal when finished
        } else {
            qDebug() << "Error opening serial port for sending.";
        }
        qDebug() << "Send Thread done\n";
    }

private:
    QSerialPort *COMPORT;
    bool retransmit_bool;
};

#endif // THRE_H
