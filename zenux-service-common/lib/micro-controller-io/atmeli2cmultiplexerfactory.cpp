#include "atmeli2cmultiplexerfactory.h"
#include <i2cmuxer.h>
#include <xmlconfigreader.h>

I2cMuxerInterface::Ptr AtmelI2cMultiplexerFactory::createClampMuxer(QString deviceNode, ushort i2cMuxAdress, quint8 ctrlChannel)
{
    return I2cMuxerInterface::Ptr(new I2cMuxer(deviceNode, i2cMuxAdress, (ctrlChannel-4) | 8, 0));
}

I2cMuxerInterface::Ptr AtmelI2cMultiplexerFactory::createNullMuxer()
{
    return I2cMuxerInterface::Ptr(new I2cMuxerNull());
}
