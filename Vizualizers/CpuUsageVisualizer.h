#ifndef CPUUSAGEVISUALIZER_H
#define CPUUSAGEVISUALIZER_H

#include <QObject>
#include <QColor>

#include <windows.h>

class QTimer;
class QSerialPort;
class QMutex;

class CpuUsageVisualizer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString portName READ portName WRITE setPortName NOTIFY portNameChanged)
public:
    explicit CpuUsageVisualizer(QObject *parent = nullptr);

    const QString &portName() const;

public slots:
    void start();
    void stop();
    void setPortName(const QString &portName);

signals:
    void portNameChanged(QString portName);

protected:
    void updateColor();

protected slots:
    void timeout();

private:
    unsigned long long FileTimeToInt64(const FILETIME & ft);

    int fetchCpuPercentUsage();
    QColor colorByPercent(double cpuUsagePercent);

    QSerialPort *m_serialPort;
    QTimer *m_updateTimer;
    QMutex *m_mutex;
    QString m_portName;

    int m_lastCpuUsage = 0;

    quint64 m_previousTotalTicks = 0;
    quint64 m_previousIdleTicks = 0;
};

#endif // CPUUSAGEVISUALIZER_H
