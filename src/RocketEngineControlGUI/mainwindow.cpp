#include <QtCharts/QtCharts>
#include <QSerialPort>

#include "./ui_mainwindow.h"
#include "mainwindow.h"
#define DATA_OUTPUT_PATH "../../data"

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

    this->setupConnections();

    this->userAlert = new AlertDialog();
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
    delete userAlert;
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


    this->currentState = EngineStates::PENDING_SHUTDOWN;
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

    // Alert the user
    userAlert->setAlertDescription("Communications Lost");
    userAlert->setAlertTitle(
        "The command: "
        + QString::fromStdString(command)
        + " has failed to be sent and acknowledged by the test stand."
        + " Please check the communications hardware and try again. "
        + " Engine shutdown has been started. "
    );
    userAlert->show();

    currentState = EngineStates::CONNECTION_FAILURE;

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

    userAlert->setAlertDescription("Serial Port Read Error");
    userAlert->setAlertTitle("There was a read error in the serial port. Please check the USB-Serial device and try again. ");
    userAlert->show();
}

void MainWindow::handleResourceErrorOccurred()
{
    logger.logEvent(EventType::SerialError, "Resource error, try restarting the computer or UART device.");
    // Alert the user to the error somehow

    userAlert->setAlertDescription("Serial Resource Error");
    userAlert->setAlertTitle("There was a serial resource error. This is likely due to another program"
                             " trying to read the serial port. Please close other programs and try again.");
    userAlert->show();
}

void MainWindow::handlePermissionErrorOccurred()
{
    logger.logEvent(EventType::SerialError, "Permissions Error: Check user serial permissions and restart software.");
    // Alert the user to the error somehow

    userAlert->setAlertDescription("Serial Permissions Error");
    userAlert->setAlertTitle("The current user does not have the permissions to access the specified serial port"
                             " please check your permissions and try again.");
    userAlert->show();
}

void MainWindow::handleGenericErrorOccurred(QSerialPort::SerialPortError error)
{
    logger.logEvent(EventType::SerialError, "Other Serial Error Occurred, Error Code: " + QString::number(error));
    // Alert the user to the error somehow

    userAlert->setAlertDescription("Generic Serial Error Occurred");
    userAlert->setAlertTitle(
        "An undefined serial error with code: "
        + QString::number(error) + "occurred. Please check serial hardware and try again.");
    userAlert->show();
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

    ui->FuelInletChart->setChartTitle("Fuel Inlet Temp");
    ui->FuelInletChart->setChartType(ChartType::Temperature);
    ui->OxidizerInletChart->setChartTitle("Oxidizer Inlet Temp");
    ui->OxidizerInletChart->setChartType(ChartType::Temperature);

    ui->EngineThroatChart->setChartTitle("Throat Temperature");
    ui->EngineThroatChart->setChartType(ChartType::Temperature);
    ui->NozzleExitChart->setChartTitle("Nozzle Exit Temperature");
    ui->NozzleExitChart->setChartType(ChartType::Temperature);

    ui->OxidizerTankPressureChart->setChartTitle("Oxidizer Tank Pressure");
    ui->OxidizerTankPressureChart->setChartType(ChartType::Pressure);
    ui->OxidizerLinePressureChart->setChartTitle("Oxidizer Line Pressure");
    ui->OxidizerLinePressureChart->setChartType(ChartType::Pressure);
    ui->FuelTankPressureChart->setChartTitle("Fuel Tank Pressure");
    ui->FuelTankPressureChart->setChartType(ChartType::Pressure);
    ui->FuelLinePressureChart->setChartTitle("Fuel Line Pressure");
    ui->FuelLinePressureChart->setChartType(ChartType::Pressure);
    ui->CombustionChamberPressureChart->setChartTitle("Combustion Chamber Pressure");
    ui->CombustionChamberPressureChart->setChartType(ChartType::Pressure);
    ui->FuelFeedPressureChart->setChartTitle("Kerosene Feed Line Pressure");
    ui->FuelFeedPressureChart->setChartType(ChartType::Pressure);
}

void MainWindow::setupConnections()
{
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
        &MainWindow::handleCommandAttempt,
        Qt::QueuedConnection
        );

    connect(
        this->commsCenter,
        &SerialWorker::commandFailed,
        this,
        &MainWindow::handleCommandFailed,
        Qt::QueuedConnection
        );

    connect(
        this->commsCenter,
        &SerialWorker::commandSuccess,
        this,
        &MainWindow::handleCommandSuccess,
        Qt::QueuedConnection
        );

    connect(
        this->commsCenter,
        &SerialWorker::dataAvailable,
        this,
        &MainWindow::handleDataAvailable,
        Qt::QueuedConnection
        );

    connect(
        this->commsCenter,
        &SerialWorker::corruptedData,
        this,
        &MainWindow::handleCorruptedData,
        Qt::QueuedConnection
        );

    connect(
        this->commsCenter,
        &SerialWorker::portOpenFailed,
        this,
        &MainWindow::handlePortOpenFailed,
        Qt::QueuedConnection
        );

    connect(
        this->commsCenter,
        &SerialWorker::portOpenSuccess,
        this,
        &MainWindow::handlePortOpenSuccess,
        Qt::QueuedConnection
        );

    connect(
        this->commsCenter,
        &SerialWorker::readErrorOccurred,
        this,
        &MainWindow::handleReadErrorOccurred,
        Qt::QueuedConnection
        );

    connect(
        this->commsCenter,
        &SerialWorker::resourceErrorOccurred,
        this,
        &MainWindow::handleResourceErrorOccurred,
        Qt::QueuedConnection
        );

    connect(
        this->commsCenter,
        &SerialWorker::permissionErrorOccurred,
        this,
        &MainWindow::handlePermissionErrorOccurred,
        Qt::QueuedConnection
        );

    connect(
        this->commsCenter,
        &SerialWorker::genericErrorOccurred,
        this,
        &MainWindow::handleGenericErrorOccurred,
        Qt::QueuedConnection
        );
}
