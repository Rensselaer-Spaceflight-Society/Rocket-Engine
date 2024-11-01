#include "loghandler.h"

LogHandler::LogHandler(
    QString outputPath,
    QString dataFileName,
    QString eventFileName,
    QString corruptedDataFileName
)
{
    QString datetime = QDateTime::currentDateTime().toString("yyyy-mm-dd-HH:mm:ss");
    if(dataFileName == "default")
    {
        dataFileName = QString("sensor-data-%1.csv").arg(datetime);
    }

    if(eventFileName == "default")
    {
        eventFileName = QString("sensor-data-%1.csv").arg(datetime);
    }

    if(corruptedDataFileName == "default")
    {
        corruptedDataFileName = QString("sensor-data-%1.csv").arg(datetime);
    }

    QString dataFilePath = outputPath + "/" + dataFileName;
    QString eventFilePath = outputPath + "/" + eventFileName;
    QString corruptedDataFilePath = outputPath + "/" + corruptedDataFileName;

    dataLog.setFileName(dataFilePath);
    eventLog.setFileName(eventFilePath);
    corruptionLog.setFileName(corruptedDataFilePath);

    bool fileOpened = openFile(this->dataLog);

    if(!fileOpened)
    {
        throw std::runtime_error("Sensor Data Log failed to Open");
    }

    fileOpened = openFile(this->eventLog);

    if(!fileOpened)
    {
        throw std::runtime_error("Event Log failed to Open");
    }

    fileOpened = openFile(this->corruptionLog);

    if(!fileOpened)
    {
        throw std::runtime_error("Corrupted Data Log failed to Open");
    }

}

LogHandler::~LogHandler()
{
    eventLog.close();
    dataLog.close();
    corruptionLog.close();
}

void LogHandler::logData(const QSharedPointer<SensorData> data)
{

}

void LogHandler::logEvent(EventType eventType, const QString & message)
{

}

void LogHandler::logCorruptedData(const QSharedPointer<QByteArray> corruptedData)
{

}

bool LogHandler::openFile(QFile & fileToOpen)
{
    return false;
}
