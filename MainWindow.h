#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QComboBox;
class QPushButton;

class CpuUsageVisualizer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected slots:
    void portChanged(const int index);

private:
    QComboBox *m_portsSelect;
    QPushButton *m_startBtn;

    CpuUsageVisualizer *m_cpuUsageVisualizer;
};

#endif // MAINWINDOW_H
