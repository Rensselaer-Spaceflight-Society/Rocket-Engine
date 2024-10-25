#include <QtCharts/QtCharts>
#include <QSerialPort>

#include "./ui_mainwindow.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->configureCharts();
    this->handleSerialPortRefresh();

    // TODO: Finalize Comms Details with Firmware and Software
    this->commsPort = new QSerialPort();


    this->commandSender = new SerialDataWriter(this->commsPort);
    this->commandSender->wait();
    this->commandSender->start();

    // Disable the Abort and Countdown Button until a connection is established
    ui->AbortButton->setDisabled(true);
    ui->StartCountdown->setDisabled(true);

    connect(this->ui->RefreshSerialPorts, &QPushButton::clicked, this, &MainWindow::handleSerialPortRefresh);
    connect(this->ui->AbortButton, &QPushButton::clicked, this, &MainWindow::handleShutdown);
    connect(this->ui->SerialPortDropdown, &QComboBox::currentIndexChanged, this, &MainWindow::handleSerialPortSelection);
    connect(this, &MainWindow::issueCommand, this->commandSender, &SerialDataWriter::issueCommand);
    connect(this, &MainWindow::startPings, this->commandSender, &SerialDataWriter::setStartPings);
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
    this->commandSender->exit();
    if(commsPort->isOpen()) commsPort->close();
    delete commandSender;
    delete commsPort;
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
    if(this->commsPort->isOpen()) this->commsPort->close();
    // Adjust the index by -1 to account for the "Select a Serial Port Option"
    if(index-1 < 0) return;
    this->commsPort->setPort(availableSerialPorts[index-1]);
    if(!this->commsPort->open(QIODevice::ReadWrite))
    {
        ui->ConnectionStatus->setText("Serial Port Failed to Open");
    }
    else
    {
        ui->ConnectionStatus->setText("Serial Port Opened");
        // Attempt connection with the test stand
        std::string connectionCommand = CONTROL_ACTIVE_COMMAND;
        emit issueCommand(connectionCommand);
    }
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

