#include <QtCharts/QtCharts>
#include <QSerialPort>

#include "./ui_mainwindow.h"
#include "mainwindow.h"
#define DATA_OUTPUT_PATH "../data"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), logger(DATA_OUTPUT_PATH)
{
    ui->setupUi(this);
    this->configureCharts();
    this->handleSerialPortRefresh();

    commsCenter = new SerialWorker(this);

    // Disable the Abort and Countdown Button until a connection is established
    ui->AbortButton->setDisabled(true);
    ui->StartCountdown->setDisabled(true);

    connect(
        this->ui->RefreshSerialPorts,
        &QPushButton::clicked,
        this,
        &MainWindow::handleSerialPortRefresh
    );

    connect(
        this->ui->AbortButton,
        &QPushButton::clicked,
        this,
        &MainWindow::handleShutdown
    );

    connect(
        this->ui->SerialPortDropdown,
        &QComboBox::currentIndexChanged,
        this,
        &MainWindow::handleSerialPortSelection
    );

    connect(
        this->commsCenter,
        &SerialWorker::commandAttempt,
        this,
        &MainWindow::handleCommandAttempt
    );

    connect(
        this->commsCenter,
        &SerialWorker::commandFailed,
        this,
        &MainWindow::handleCommandFailed
    );

    connect(
        this->commsCenter,
        &SerialWorker::commandSuccess,
        this,
        &MainWindow::handleCommandSuccess
    );

    connect(
        this->commsCenter,
        &SerialWorker::dataAvailable,
        this,
        &MainWindow::handleDataAvailable
    );

    connect(
        this->commsCenter,
        &SerialWorker::corruptedData,
        this,
        &MainWindow::handleCorruptedData
    );

    connect(
        this->commsCenter,
        &SerialWorker::portOpenFailed,
        this,
        &MainWindow::handlePortOpenFailed
    );

    connect(
        this->commsCenter,
        &SerialWorker::portOpenSuccess,
        this,
        &MainWindow::handlePortOpenSuccess
    );

    connect(
        this->commsCenter,
        &SerialWorker::readErrorOccurred,
        this,
        &MainWindow::handleReadErrorOccurred
    );

    connect(
        this->commsCenter,
        &SerialWorker::resourceErrorOccurred,
        this,
        &MainWindow::handleResourceErrorOccurred
    );

    connect(
        this->commsCenter,
        &SerialWorker::permissionErrorOccurred,
        this,
        &MainWindow::handlePermissionErrorOccurred
    );

    connect(
        this->commsCenter,
        &SerialWorker::genericErrorOccurred,
        this,
        &MainWindow::handleGenericErrorOccurred
    );
}

void MainWindow::keyPressEvent(QKeyEvent* keyEvent)
{
    if(keyEvent->key() == Qt::Key_Backspace
        && this->ui->AbortButton->isEnabled())
    {
        this->handleShutdown();
    }

    QMainWindow::keyPressEvent(keyEvent);
}

MainWindow::~MainWindow()
{
    delete commsCenter;
    delete ui;
}

void MainWindow::handleSerialPortRefresh()
{
    ui->SerialPortDropdown->clear();
    ui->SerialPortDropdown->addItem("Select a Serial Port");
    ui->SerialPortDropdown->addItems(this->getSerialPorts());
}

void MainWindow::handleShutdown()
{
    ui->AbortButton->setDisabled(true);
    ui->StartCountdown->setDisabled(true);
    ui->AbortButton->setText("Shutdown Started");
    ui->AbortButton->setStyleSheet("#AbortButton {background-color: rgb(119, 118, 123); color: rgb(255, 255, 255);}");

    std::string shutdown = SHUTDOWN_COMMAND;
    emit issueCommand(shutdown);
}

void MainWindow::handleSerialPortSelection(int index)
{
    // Adjust the index by -1 to account for the "Select a Serial Port Option"
    if(index-1 < 0) return;
    emit serialPortChanged(availableSerialPorts[index-1]);
}

void MainWindow::handleCommandAttempt(std::string command)
{
    // Log the attempt since the serial worker should handle repetition
    logger.logEvent(EventType::CommandSent, QString::fromStdString(command)+ " Sent to Motor");
}

void MainWindow::handleCommandFailed(std::string command)
{
    // Log the failure in comms
    logger.logEvent(
        EventType::CommandFailed,
        QString::fromStdString(command) + " was sent "
            + QString::number(MAX_COMMAND_RETRIES)
            + " times without acknowledgement."
    );

    // Alert the user to check hardware and reconnect
    throw std::runtime_error("NOT IMPLEMENTED");

    // Try to shutdown assuming the connection from the rocket is the one that is severed
    handleShutdown();
}

void MainWindow::handleCommandSuccess(std::string command)
{
    // Log the success
    logger.logEvent(
        EventType::AcknowledgementReceived,
        QString("Received Ack for: ") + QString::fromStdString(command)
    );

    // Then we want to transition the state to the next state and update any text

    throw std::runtime_error("NOT IMPLEMENTED");
}

void MainWindow::handleDataAvailable(const QSharedPointer<SensorData> data)
{
    // Log the data in the data log
    logger.logData(data);

    // Update the graphs and the data table
    throw std::runtime_error("NOT IMPLEMENTED");
}

void MainWindow::handleCorruptedData(const QSharedPointer<QByteArray> data)
{
    // Log the corrupted data event
    logger.logEvent(
        EventType::CorruptedData,
        QString::number(data->size())+ " Bytes of Corrupted Data recieved and logged. "
    );

    // Log the corrupted data
    logger.logCorruptedData(data);
}

void MainWindow::handlePortOpenFailed()
{
    // Log the event
    logger.logEvent(
        EventType::SerialError,
        "Serial Port Failed to Open."
    );

    // Update the screen
    this->ui->ConnectionStatus->setText("Serial Port Failed to Open");
}

void MainWindow::handlePortOpenSuccess()
{
    logger.logEvent(EventType::Info, "Serial Port opened Successfully");

    // Update the screen

    this->ui->ConnectionStatus->setText("Serial Port Opened");
}

// Serial Error Handlers
void MainWindow::handleReadErrorOccurred()
{
    logger.logEvent(EventType::SerialError, "Read error occured. Check Hardware.");
    // Alert the user to the error somehow
    throw std::runtime_error("NOT IMPLEMENTED");
}

void MainWindow::handleResourceErrorOccurred()
{
    logger.logEvent(EventType::SerialError, "Resource error, try restarting the computer or UART device.");
    // Alert the user to the error somehow
    throw std::runtime_error("NOT IMPLEMENTED");
}

void MainWindow::handlePermissionErrorOccurred()
{
    logger.logEvent(EventType::SerialError, "Permissions Error: Check user serial permissions and restart software.");
    // Alert the user to the error somehow
    throw std::runtime_error("NOT IMPLEMENTED");
}

void MainWindow::handleGenericErrorOccurred(QSerialPort::SerialPortError error)
{
    logger.logEvent(EventType::SerialError, "Other Serial Error Occurred, Error Code: " + QString::number(error));
    // Alert the user to the error somehow
    throw std::runtime_error("NOT IMPLEMENTED");
}


QStringList MainWindow::getSerialPorts()
{

    QStringList portDropdownOptions;
    this->availableSerialPorts = QSerialPortInfo::availablePorts();
    for(auto const & port: this->availableSerialPorts)
    {
        if(port.hasVendorIdentifier())
        {
            portDropdownOptions.append(port.portName() + ": " + port.manufacturer());
        }
    }
    return portDropdownOptions;
}

void MainWindow::configureCharts()
{
    ui->LoadCellChart->setChartTitle("Load Cell");
    ui->LoadCellChart->setChartType(ChartType::Force);

    ui->FuelInlet1Chart->setChartTitle("Fuel Inlet 1 Temp");
    ui->FuelInlet1Chart->setChartType(ChartType::Temperature);
    ui->FuelInlet2Chart->setChartTitle("Fuel Inlet 2 Temp");
    ui->FuelInlet2Chart->setChartType(ChartType::Temperature);

    ui->OxidizerInlet1Chart->setChartTitle("Oxidizer Inlet 1 Temp");
    ui->OxidizerInlet1Chart->setChartType(ChartType::Temperature);
    ui->OxidizerInlet2Chart->setChartTitle("Oxidizer Inlet 2 Temp");
    ui->OxidizerInlet2Chart->setChartType(ChartType::Temperature);

    ui->OxidizerTankPressureChart->setChartTitle("Oxidizer Tank Pressure");
    ui->OxidizerTankPressureChart->setChartType(ChartType::Pressure);
    ui->OxidizerLinePressureChart->setChartTitle("Oxidizer Line Pressure");
    ui->OxidizerLinePressureChart->setChartType(ChartType::Pressure);
    ui->FuelTankPressureChart->setChartTitle("Fuel Tank Pressure");
    ui->FuelTankPressureChart->setChartType(ChartType::Pressure);
    ui->FuelLinePressureChart->setChartTitle("Fuel Line Pressure");
    ui->FuelLinePressureChart->setChartType(ChartType::Pressure);
    ui->NitrogenTankPressureChart->setChartTitle("Nitrogen Tank Pressure");
    ui->NitrogenTankPressureChart->setChartType(ChartType::Pressure);
    ui->FuelFeedPressureChart->setChartTitle("Nitrogen Line Pressure");
    ui->FuelFeedPressureChart->setChartType(ChartType::Pressure);
}

