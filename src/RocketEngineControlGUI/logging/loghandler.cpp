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
        eventFileName = QString("event-log-%1.csv").arg(datetime);
    }

    if(corruptedDataFileName == "default")
    {
        corruptedDataFileName = QString("corrupted-data-%1.csv").arg(datetime);
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
        throw std::runtime_error("Sensor Data Log failed to Open, check that the destination folder exists and you have"
                                 "permissions to write to the folder.");
    }

    fileOpened = openFile(this->eventLog);

    if(!fileOpened)
    {
        throw std::runtime_error("Event Log failed to Open, check that the destination folder exists and you have"
                                 "permissions to write to the folder.");
    }

    fileOpened = openFile(this->corruptionLog);

    if(!fileOpened)
    {
        throw std::runtime_error("Corrupted Data Log failed to Open, check that the destination folder exists and you have"
                                 "permissions to write to the folder.");
    }

    dataStream.setDevice(&dataLog);
    eventStream.setDevice(&eventLog);
    corruptionStream.setDevice(&corruptionLog);

    // Setup Log File headers

    dataStream << "UNIX Time (ms), "<< "Local Time, " << "Load Cell (N), "
               << "Kerosene Inlet (c), " << "Oxidizer Inlet (c), "
               << "Throat (c), " << "Nozzle Outlet (c), "
               << "Combustion Chamber (kPa), " << "Kerosene Feed Line (kPa), "
               << "Kerosene Tank (kPa), " << "Kerosene Line (kPa), "
               << "Oxidizer Tank (kPa), " << "Oxidizer Line (kPa)"
               << "\n";

    eventStream << "UNIX Time (ms), " << "Local Time, " << "Event Type, "
                << "Message " << "\n";

    corruptionStream << "UNIX Time (ms), " << "Local Time, " << "Byte Length, "
                     << "Data" << "\n";
}

LogHandler::~LogHandler()
{
    eventLog.close();
    dataLog.close();
    corruptionLog.close();
}

void LogHandler::logData(const QSharedPointer<SensorData> data)
{
    qint64 unixTime = QDateTime::currentSecsSinceEpoch();
    QString localTime = QDateTime::currentDateTime().toString("HH:mm:ss.zzz");

    dataStream << unixTime << ", " << localTime << ", " << data->loadCell
               << ", " << data->thermocouple[0] << ", " << data->thermocouple[1]
               << ", " << data->thermocouple[2] << ", " << data->thermocouple[3]
               << ", " << data->pressureTransducer[0] << ", " << data->pressureTransducer[1]
               << ", " << data->pressureTransducer[2] << ", " << data->pressureTransducer[3]
               << ", " << data->pressureTransducer[4] << ", " << data->pressureTransducer[5]
               << "\n";

}

void LogHandler::logEvent(EventType eventType, const QString & message)
{
    qint64 unixTime = QDateTime::currentSecsSinceEpoch();
    QString localTime = QDateTime::currentDateTime().toString("HH:mm:ss.zzz");
    QString messageType;

    switch(eventType){
        case EventType::AcknowledgementReceived:
            messageType = "[ACK]";
            break;
        case EventType::CommandFailed:
            messageType = "[COMMAND FAIL]";
            break;
        case EventType::CommandSent:
            messageType = "[COMMAND SENT]";
            break;
        case EventType::SerialError:
            messageType = "[SERIAL ERROR]";
            break;
        case EventType::CorruptedData:
            messageType = "[CORRUPTED DATA]";
            break;
        case EventType::Debug:
            messageType = "[DEBUG]";
            break;
        case EventType::Info:
            messageType = "[INFO]";
            break;
        case EventType::Warning:
            messageType = "[WARNING]";
            break;
        case EventType::Error:
            messageType = "[ERROR]";
            break;
    }

    eventStream << unixTime << ", " << localTime << ", "
                << messageType << "," << message << "\n";
}

void LogHandler::logCorruptedData(const QSharedPointer<QByteArray> corruptedData)
{
    qint64 unixTime = QDateTime::currentSecsSinceEpoch();
    QString localTime = QDateTime::currentDateTime().toString("HH:mm:ss.zzz");

    corruptionStream << unixTime << ", " << localTime << ", "
                     << (*corruptedData).size() << (*corruptedData).toHex();

}

bool LogHandler::openFile(QFile & fileToOpen)
{
    return fileToOpen.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
}
