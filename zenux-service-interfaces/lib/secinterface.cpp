#include "secinterface.h"
#include "secinterface_p.h"

namespace Zera {

cSECInterface::cSECInterface() :
    d_ptr(new cSECInterfacePrivate(this))
{
}

cSECInterface::~cSECInterface()
{
    delete d_ptr;
}

void cSECInterface::setClientSmart(ProxyClientPtr client)
{
    Q_D(cSECInterface);
    return d->setClientSmart(client);
}

quint32 cSECInterface::scpiCommand(const QString &scpi)
{
    Q_D(cSECInterface);
    return d->transparentCommand(scpi);
}

quint32 cSECInterface::getChannelCatalog()
{
    Q_D(cSECInterface);
    return d->transparentCommand("ECALCULATOR:CHANNEL:CATALOG?");
}


quint32 cSECInterface::setECalcUnit(int n)
{
    Q_D(cSECInterface);
    return d->setECalcUnit(n);
}


quint32 cSECInterface::freeECalcUnits()
{
    Q_D(cSECInterface);
    return d->freeECalcUnits();
}


quint32 cSECInterface::writeRegister(const QString &chnname, quint8 reg, quint32 value)
{
    Q_D(cSECInterface);
    return d->writeRegister(chnname, reg, value);
}


quint32 cSECInterface::readRegister(const QString &chnname, quint8 reg)
{
    Q_D(cSECInterface);
    return d->readRegister(chnname, reg);
}


quint32 cSECInterface::setSync(const QString &chnname, const QString &syncchn)
{
    Q_D(cSECInterface);
    return d->setSync(chnname, syncchn);
}


quint32 cSECInterface::setMux(const QString &chnname, const QString &inpname)
{
    Q_D(cSECInterface);
    return d->setMux(chnname, inpname);
}


quint32 cSECInterface::setCmdid(const QString &chnname, quint8 cmdid)
{
    Q_D(cSECInterface);
    return d->setCmdid(chnname, cmdid);
}


quint32 cSECInterface::start(const QString &chnname)
{
    Q_D(cSECInterface);
    return d->start(chnname);
}


quint32 cSECInterface::stop(const QString &chnname)
{
    Q_D(cSECInterface);
    return d->stop(chnname);
}


quint32 cSECInterface::intAck(const QString &chnname, quint8 interrupt)
{
    Q_D(cSECInterface);
    return d->intAck(chnname, interrupt);
}


quint32 cSECInterface::registerNotifier(const QString &query)
{
    Q_D(cSECInterface);
    return d->registerNotifier(query);
}


quint32 cSECInterface::unregisterNotifiers()
{
    Q_D(cSECInterface);
    return d->unregisterNotifiers();
}

}
