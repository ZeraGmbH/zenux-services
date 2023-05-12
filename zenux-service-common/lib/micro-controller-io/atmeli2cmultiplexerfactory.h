#ifndef ATMELI2CMULTIPLEXERFACTORY_H
#define ATMELI2CMULTIPLEXERFACTORY_H

#include "i2cmuxernull.h"

class AtmelI2cMultiplexerFactory
{
public:
    static I2cMuxerInterface::Ptr createClampMuxer(QString deviceNode, ushort i2cMuxAdress, quint8 ctrlChannel);
    static I2cMuxerInterface::Ptr createNullMuxer();
};

#endif // ATMELI2CMULTIPLEXERFACTORY_H
