#ifndef TEST_I2C_DSPGENERATOR_H
#define TEST_I2C_DSPGENERATOR_H

#include <QObject>

class test_i2c_dspgenerator : public QObject
{
    Q_OBJECT
private slots:
    void getAmplitudeCmd();
    void setAmplitudeCmd();
};

#endif // TEST_I2C_DSPGENERATOR_H
