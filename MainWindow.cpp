#include "MainWindow.h"
#include <QSerialPortInfo>
#include <QLayout>
#include <QComboBox>
#include <QPushButton>
#include <QMessageBox>


#include <Vizualizers/CpuUsageVisualizer.h>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_portsSelect = new QComboBox();
    m_startBtn = new QPushButton(tr("Старт"));

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(m_portsSelect);
    mainLayout->addWidget(m_startBtn);

    QWidget *mainWidget = new QWidget();
    mainWidget->setLayout(mainLayout);

    setCentralWidget(mainWidget);

    foreach (const QSerialPortInfo &portInfo, QSerialPortInfo::availablePorts())
    {
        m_portsSelect->addItem(tr("%1 (%2)").arg(portInfo.portName()).arg(portInfo.description()), portInfo.portName());
    }

    m_cpuUsageVisualizer = new CpuUsageVisualizer(this);

    connect(m_portsSelect, SIGNAL(currentIndexChanged(int)),
            this, SLOT(portChanged(int)));

    connect(m_startBtn, SIGNAL(clicked(bool)),
            m_cpuUsageVisualizer, SLOT(start()));
}

MainWindow::~MainWindow()
{

}

void MainWindow::portChanged(const int index)
{
    m_cpuUsageVisualizer->setPortName(m_portsSelect->itemData(index).toString());
}
