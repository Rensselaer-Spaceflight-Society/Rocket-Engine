#ifndef SERIALDATAWRITER_H
#define SERIALDATAWRITER_H

#include <queue>

#include <QSerialPort>
#include <QThread>

#include "command.h"

#define TIME_PER_PING_MS 50
#define PING_COMMAND "PingPong"
#define BYTES_IN_COMMAND 8

class SerialDataWriter : public QThread
{
    Q_OBJECT

public:
    explicit SerialDataWriter(QSerialPort* tx, QObject *parent = nullptr);
    ~SerialDataWriter();
    void run() override;

public slots:
    void addNewCommandToQueue(const Command & command);
    void setStartPings();

signals:
    void sentCommand(Command command);

private:
    QSerialPort * serialPort;
    std::priority_queue<Command> * commandQueue;
    bool startPings = false;
    size_t timeSinceLastPing = 50;
};

#endif // SERIALDATAWRITER_H
