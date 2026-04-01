#ifndef TEST_I2C_UTILITIES_H
#define TEST_I2C_UTILITIES_H

#include <QObject>

class test_i2c_utilities : public QObject
{
    Q_OBJECT
private slots:
    void serializeFloat();
    void deserializeFloat();
};

#endif // TEST_I2C_UTILITIES_H
