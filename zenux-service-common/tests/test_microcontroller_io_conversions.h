#ifndef TEST_MICROCONTROLLER_IO_CONVERSIONS_H
#define TEST_MICROCONTROLLER_IO_CONVERSIONS_H

#include <QObject>

class test_microcontroller_io_conversions : public QObject
{
    Q_OBJECT
private slots:
    void serializeFloatToMicroController();
    void generatorBitmaskFromChannelMNames();
    void controllerInternalChannelNoFromChannelMName();
};

#endif // TEST_MICROCONTROLLER_IO_CONVERSIONS_H
