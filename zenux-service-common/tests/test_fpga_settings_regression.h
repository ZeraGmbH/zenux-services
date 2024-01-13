#ifndef TEST_ALLSERVERS_FPGA_DEVICE_NODES_H
#define TEST_ALLSERVERS_FPGA_DEVICE_NODES_H

#include <QObject>

class test_fpga_settings_regression : public QObject
{
    Q_OBJECT
private slots:
    void com5003d();
    void mt310s2d();
    void sec1000d();
    void zdsp1dd();
};

#endif // TEST_ALLSERVERS_FPGA_DEVICE_NODES_H
