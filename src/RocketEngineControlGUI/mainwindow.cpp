#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QtCharts/QtCharts>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->configureCharts();
    this->handleSerialPortRefresh();
    connect(this->ui->RefreshSerialPorts, &QPushButton::clicked, this, &MainWindow::handleSerialPortRefresh);
    connect(this->ui->AbortButton, &QPushButton::clicked, this, &MainWindow::handleShutdown);
}

void MainWindow::handleSerialPortRefresh()
{
 ui->SerialPortDropdown->addItems(this->getSerialPorts());
}

// TODO: Needs Testing with Actual Hardware
QStringList MainWindow::getSerialPorts()
{
    QStringList ports;
    QList<QSerialPortInfo> openPorts = QSerialPortInfo::availablePorts();
    for(auto const & port: openPorts){
        if(port.hasVendorIdentifier()) ports.append(port.manufacturer());
    }
    return ports;
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

void MainWindow::handleShutdown()
{
    ui->AbortButton->setDisabled(true);
    ui->AbortButton->setText("Shutdown Started");
    ui->AbortButton->setStyleSheet("#AbortButton {\n	background-color: rgb(119, 118, 123); \n color: rgb(255, 255, 255);}");
}

MainWindow::~MainWindow()
{
    delete ui;
}
