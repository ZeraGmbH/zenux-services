#ifndef MOCKI2CEEPROMIOFACTORY_H
#define MOCKI2CEEPROMIOFACTORY_H

#include <i2ceepromiofactory.h>

class MockI2cEEpromIoFactory : public I2cEEpromIoFactory
{
public:
    static void enableMock();
    static void disableMock();
};

#endif // MOCKI2CEEPROMIOFACTORY_H
