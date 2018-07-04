#include "CpuUsageVisualizer.h"
#include <QSerialPort>
#include <QTimer>
#include <QtConcurrent>
#include <QEventLoop>
#include <QThread>
#include <QMutex>
#include <QMutexLocker>

CpuUsageVisualizer::CpuUsageVisualizer(QObject *parent) :
    QObject(parent)
{
    m_updateTimer = new QTimer(this);
    m_updateTimer->setInterval(3000);

    m_serialPort = new QSerialPort();
    m_serialPort->setBaudRate(9600);
    m_serialPort->setDataBits(QSerialPort::Data8);
    m_serialPort->setParity(QSerialPort::NoParity);
    m_serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort->setFlowControl(QSerialPort::NoFlowControl);

    m_mutex = new QMutex();

    connect(m_updateTimer, SIGNAL(timeout()),
            this, SLOT(timeout()));
}

void CpuUsageVisualizer::start()
{
    m_updateTimer->start();
}

void CpuUsageVisualizer::stop()
{
    m_updateTimer->stop();
}

const QString &CpuUsageVisualizer::portName() const
{
    return m_portName;
}

void CpuUsageVisualizer::setPortName(const QString &portName)
{
    if (m_portName == portName)
        return;

    m_portName = portName;
    emit portNameChanged(m_portName);
}

void CpuUsageVisualizer::updateColor()
{
    QElapsedTimer timer;
    timer.start();
    QMutexLocker locker(m_mutex);
    Q_UNUSED(locker);

    const int cpuUsage = fetchCpuPercentUsage();
    qWarning() << "elapsed1:" << timer.elapsed();
    qWarning() << "elapsed2:" << timer.elapsed();

    qWarning() << cpuUsage;

    if(m_serialPort->portName() != m_portName)
    {
        m_serialPort->close();
        m_serialPort->setPortName(m_portName);
    }

    if(!m_serialPort->isOpen())
    {
        if(!m_serialPort->open(QIODevice::ReadWrite))
        {
            // Вызывать сигнал с ошибкой
            qWarning() << m_serialPort->errorString();
            stop();
            return;
        }
    }

    if (m_serialPort->isOpen() && m_serialPort->isWritable())
    {
        const bool isIncrement = (m_lastCpuUsage <= cpuUsage);

        for(int percent = m_lastCpuUsage; isIncrement ? (percent <= cpuUsage)
            : (percent >= cpuUsage); isIncrement ? percent++ : percent--)
        {
            const QColor color = colorByPercent(percent);

            QByteArray array;
            QDataStream stream(&array, QIODevice::WriteOnly);


            //            QPalette p = palette();
            //            p.setColor(QPalette::Background, color);
            //            setAutoFillBackground(true);
            //            setPalette(p);

            for(int i = 0; i < 30; i++)
            {
                stream << (unsigned char)color.red() << (unsigned char)color.green() << (unsigned char)color.blue();
            }

            m_serialPort->write(array);
            m_serialPort->flush();

            m_serialPort->waitForBytesWritten(-1);
        }

        m_lastCpuUsage = cpuUsage;
    }
    qWarning() << "elapsed3:" << timer.elapsed();
}

void CpuUsageVisualizer::timeout()
{
    updateColor();
}

unsigned long long CpuUsageVisualizer::FileTimeToInt64(const FILETIME &ft)
{
    return (((unsigned long long)(ft.dwHighDateTime))<<32)|((unsigned long long)ft.dwLowDateTime);
}


int CpuUsageVisualizer::fetchCpuPercentUsage()
{
    FILETIME idleTime, kernelTime, userTime;
    qWarning() << GetSystemTimes(&idleTime, &kernelTime, &userTime);
    if(GetSystemTimes(&idleTime, &kernelTime, &userTime))
    {
        const quint64 idleTicks = FileTimeToInt64(idleTime);
        const quint64 totalTicks = FileTimeToInt64(kernelTime) + FileTimeToInt64(userTime);

        const quint64 totalTicksSinceLastTime = totalTicks - m_previousTotalTicks;
        const quint64 idleTicksSinceLastTime  = idleTicks - m_previousIdleTicks;

        m_previousTotalTicks = totalTicks;
        m_previousIdleTicks  = idleTicks;


        return (totalTicksSinceLastTime - idleTicksSinceLastTime) * 100.0 / totalTicksSinceLastTime;
    }
    return 0;
}

QColor CpuUsageVisualizer::colorByPercent(double cpuUsagePercent)
{
    const double hue = (99.0 - cpuUsagePercent) / 100.0;

    int h = int(hue * 256 * 2);
    int x = h % 0x100;

    int r = 0, g = 0, b = 0;
    switch (h / 256)
    {
    case 0: r = 255; g = x;       break;
    case 1: g = 255; r = 255 - x; break;
    }

    return QColor(r, g, b);
}
