#ifndef I2CFLASHIOFACTORYFORTEST_H
#define I2CFLASHIOFACTORYFORTEST_H

#include <i2ceepromiofactory.h>

class I2cFlashIoFactoryForTest : public I2cEEpromIoFactory
{
public:
    static void enableMockFlash();
    static void disableMockFlash();
};

#endif // I2CFLASHIOFACTORYFORTEST_H
