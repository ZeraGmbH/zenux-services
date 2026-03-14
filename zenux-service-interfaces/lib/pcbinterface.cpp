#include "pcbinterface.h"
#include "pcbinterface_p.h"

namespace Zera {

cPCBInterface::cPCBInterface() :
    d_ptr(new cPCBInterfacePrivate(this))
{
}

cPCBInterface::~cPCBInterface()
{
    delete d_ptr;
}

void cPCBInterface::setClientSmart(Zera::ProxyClientPtr client)
{
    Q_D(cPCBInterface);
    return d->setClientSmart(client);
}

quint32 cPCBInterface::scpiCommand(const QString &scpi)
{
    Q_D(cPCBInterface);
    return d->transparentCommand(scpi);
}

quint32 cPCBInterface::getChannelList()
{
    Q_D(cPCBInterface);
    return d->getChannelList();
}


quint32 cPCBInterface::getDSPChannel(const QString &chnName)
{
    Q_D(cPCBInterface);
    return d->getDSPChannel(chnName);
}


quint32 cPCBInterface::getStatus(const QString &chnName)
{
    Q_D(cPCBInterface);
    return d->getStatus(chnName);
}


quint32 cPCBInterface::resetStatus(const QString &chnName)
{
    Q_D(cPCBInterface);
    return d->resetStatus(chnName);
}


quint32 cPCBInterface::getAlias(const QString &chnName)
{
    Q_D(cPCBInterface);
    return d->getAlias(chnName);
}


quint32 cPCBInterface::getType(const QString &chnName)
{
    Q_D(cPCBInterface);
    return d->getType(chnName);
}


quint32 cPCBInterface::getUnit(const QString &chnName)
{
    Q_D(cPCBInterface);
    return d->getUnit(chnName);
}


quint32 cPCBInterface::getRange(const QString &chnName)
{
    Q_D(cPCBInterface);
    return d->getRange(chnName);
}


quint32 cPCBInterface::getRangeList(const QString &chnName)
{
    Q_D(cPCBInterface);
    return d->getRangeList(chnName);
}


quint32 cPCBInterface::getAlias(const QString &chnName, const QString &rngName)
{
    Q_D(cPCBInterface);
    return d->getAlias(chnName, rngName);
}


quint32 cPCBInterface::getType(const QString &chnName, const QString &rngName)
{
    Q_D(cPCBInterface);
    return d->getType(chnName, rngName);
}


quint32 cPCBInterface::getUrvalue(const QString &chnName, const QString &rngName)
{
    Q_D(cPCBInterface);
    return d->getUrvalue(chnName, rngName);
}


quint32 cPCBInterface::getUrvalue(const QString &chnName)
{
    Q_D(cPCBInterface);
    return d->getUrvalue(chnName);
}


quint32 cPCBInterface::getRejection(const QString &chnName, const QString &rngName)
{
    Q_D(cPCBInterface);
    return d->getRejection(chnName, rngName);
}


quint32 cPCBInterface::getOVRejection(const QString &chnName, const QString &rngName)
{
    Q_D(cPCBInterface);
    return d->getOVRejection(chnName, rngName);
}


quint32 cPCBInterface::getADCRejection(const QString &chnName, const QString &rngName)
{
    Q_D(cPCBInterface);
    return d->getADCRejection(chnName, rngName);
}


quint32 cPCBInterface::isAvail(const QString &chnName, const QString &rngName)
{
    Q_D(cPCBInterface);
    return d->isAvail(chnName, rngName);
}


quint32 cPCBInterface::getGainCorrection(const QString &chnName, const QString &rngName, double at)
{
    Q_D(cPCBInterface);
    return d->getGainCorrection(chnName, rngName, at);
}


quint32 cPCBInterface::getAdjGainCorrection(const QString &chnName, const QString &rngName, double at)
{
    Q_D(cPCBInterface);
    return d->getAdjGainCorrection(chnName, rngName, at);
}


quint32 cPCBInterface::getOffsetCorrection(const QString &chnName, const QString &rngName, double at)
{
    Q_D(cPCBInterface);
    return d->getOffsetCorrection(chnName, rngName, at);
}


quint32 cPCBInterface::getAdjOffsetCorrection(const QString &chnName, const QString &rngName, double at)
{
    Q_D(cPCBInterface);
    return d->getAdjOffsetCorrection(chnName, rngName, at);
}


quint32 cPCBInterface::getPhaseCorrection(const QString &chnName, const QString &rngName, double at)
{
    Q_D(cPCBInterface);
    return d->getPhaseCorrection(chnName, rngName, at);
}


quint32 cPCBInterface::getAdjPhaseCorrection(const QString &chnName, const QString &rngName, double at)
{
    Q_D(cPCBInterface);
    return d->getAdjPhaseCorrection(chnName, rngName, at);
}


quint32 cPCBInterface::setRange(const QString &chnName, const QString &rngName)
{
    Q_D(cPCBInterface);
    return d->setRange(chnName, rngName);
}


quint32 cPCBInterface::setMMode(const QString &mmode)
{
    Q_D(cPCBInterface);
    return d->setMMode(mmode);
}


quint32 cPCBInterface::adjustComputation()
{
    Q_D(cPCBInterface);
    return d->adjustComputation();
}


quint32 cPCBInterface::adjustStorage()
{
    Q_D(cPCBInterface);
    return d->adjustStorage();
}


quint32 cPCBInterface::adjustStorageClamp()
{
    Q_D(cPCBInterface);
    return d->adjustStorageClamp();
}


quint32 cPCBInterface::setAdjustGainStatus(const QString &chnName, const QString &rngName, int stat)
{
    Q_D(cPCBInterface);
    return d->setAdjustGainStatus(chnName, rngName, stat);
}


quint32 cPCBInterface::setAdjustPhaseStatus(const QString &chnName, const QString &rngName, int stat)
{
    Q_D(cPCBInterface);
    return d->setAdjustPhaseStatus(chnName, rngName, stat);
}


quint32 cPCBInterface::setAdjustOffsetStatus(const QString &chnName, const QString &rngName, int stat)
{
    Q_D(cPCBInterface);
    return d->setAdjustOffsetStatus(chnName, rngName, stat);
}


quint32 cPCBInterface::setGainNode(const QString &chnName, const QString &rngName, int nr, double corr, double at)
{
    Q_D(cPCBInterface);
    return d->setGainNode(chnName, rngName, nr, corr, at);
}


quint32 cPCBInterface::setPhaseNode(const QString &chnName, const QString &rngName, int nr, double corr, double at)
{
    Q_D(cPCBInterface);
    return d->setPhaseNode(chnName, rngName, nr, corr, at);
}


quint32 cPCBInterface::setOffsetNode(const QString &chnName, const QString &rngName, int nr, double corr, double at)
{
    Q_D(cPCBInterface);
    return d->setOffsetNode(chnName, rngName, nr, corr, at);

}

quint32 cPCBInterface::getSourceCat()
{
    Q_D(cPCBInterface);
    return d->transparentCommand("SOURCE:CHANNEL:CATALOG?");
}

quint32 cPCBInterface::registerNotifier(const QString &query, int notifyId, bool notifyWithValue)
{
    Q_D(cPCBInterface);
    return d->registerNotifier(query, notifyId, notifyWithValue);
}


quint32 cPCBInterface::unregisterNotifiers()
{
    Q_D(cPCBInterface);
    return d->unregisterNotifiers();
}


quint32 cPCBInterface::readServerVersion()
{
    Q_D(cPCBInterface);
    return d->readServerVersion();
}


quint32 cPCBInterface::readPCBInfo()
{
    Q_D(cPCBInterface);
    return d->readPCBInfo();
}

quint32 cPCBInterface::readChannelsConnected()
{
    Q_D(cPCBInterface);
    return d->readChannelsConnected();
}


quint32 cPCBInterface::readFPGAVersion()
{
    Q_D(cPCBInterface);
    return d->readFPGAVersion();
}


quint32 cPCBInterface::readCTRLVersion()
{
    Q_D(cPCBInterface);
    return d->readCTRLVersion();
}


quint32 cPCBInterface::readSerialNr()
{
    Q_D(cPCBInterface);
    return d->readSerialNr();
}


quint32 cPCBInterface::writeSerialNr(const QString &serNr)
{
    Q_D(cPCBInterface);
    return d->writeSerialNr(serNr);
}


quint32 cPCBInterface::getAdjustmentChksum()
{
    Q_D(cPCBInterface);
    return d->getAdjustmentChksum();
}


quint32 cPCBInterface::getPCBAdjustmentData()
{
    Q_D(cPCBInterface);
    return d->getPCBAdjustmentData();
}


quint32 cPCBInterface::setPCBAdjustmentData(const QString &xmlpcb)
{
    Q_D(cPCBInterface);
    return d->setPCBAdjustmentData(xmlpcb);
}


quint32 cPCBInterface::getClampAdjustmentData()
{
    Q_D(cPCBInterface);
    return d->getClampAdjustmentData();
}


quint32 cPCBInterface::setClampAdjustmentData(const QString &xmlclamp)
{
    Q_D(cPCBInterface);
    return d->setClampAdjustmentData(xmlclamp);
}

quint32 cPCBInterface::getAccumulatorStatus()
{
    Q_D(cPCBInterface);
    return d->getAccumulatorStatus();
}

quint32 cPCBInterface::getAccuStateOfCharge()
{
    Q_D(cPCBInterface);
    return d->getAccuStateOfCharge();
}

quint32 cPCBInterface::readData()
{
    Q_D(cPCBInterface);
    return d->readData();
}

quint32 cPCBInterface::writeData(const QString &par)
{
    Q_D(cPCBInterface);
    return d->writeData(par);
}

quint32 cPCBInterface::transparentCommand(const QString &cmd)
{
    Q_D(cPCBInterface);
    return d->transparentCommand(cmd);
}


quint32 cPCBInterface::getSampleRate()
{
    Q_D(cPCBInterface);
    return d->getSampleRate();
}


quint32 cPCBInterface::setPLLChannel(const QString &samplechnName, const QString &pllchnName)
{
    Q_D(cPCBInterface);
    return d->setPLLChannel(samplechnName, pllchnName);
}


quint32 cPCBInterface::getAliasSource(const QString &chnName)
{
    Q_D(cPCBInterface);
    return d->getAliasSource(chnName);
}


quint32 cPCBInterface::getDSPChannelSource(const QString &chnName)
{
    Q_D(cPCBInterface);
    return d->getDSPChannelSource(chnName);
}


quint32 cPCBInterface::getFormFactorSource(const QString &chnName)
{
    Q_D(cPCBInterface);
    return d->getFormFactorSource(chnName);
}


quint32 cPCBInterface::getConstantSource(const QString &chnName)
{
    Q_D(cPCBInterface);
    return d->getConstantSource(chnName);
}


quint32 cPCBInterface::setConstantSource(const QString &chnName, double constant)
{
    Q_D(cPCBInterface);
    return d->setConstantSource(chnName, constant);
}


quint32 cPCBInterface::getPowTypeSource(const QString &chnName)
{
    Q_D(cPCBInterface);
    return d->getPowTypeSource(chnName);
}


quint32 cPCBInterface::setPowTypeSource(const QString &chnName, const QString &ptype)
{
    Q_D(cPCBInterface);
    return d->setPowTypeSource(chnName, ptype);
}


quint32 cPCBInterface::getAliasSchead(const QString &chnName)
{
    Q_D(cPCBInterface);
    return d->getAliasSchead(chnName);
}


quint32 cPCBInterface::getAliasFrqinput(const QString &chnName)
{
    Q_D(cPCBInterface);
    return d->getAliasFrqinput(chnName);
}


quint32 cPCBInterface::getAdjustmentStatus()
{
    Q_D(cPCBInterface);
    return d->getAdjustmentStatus();
}


quint32 cPCBInterface::getAuthorizationStatus()
{
    Q_D(cPCBInterface);
    return d->getAuthorizationStatus();
}

quint32 cPCBInterface::resourceAliasQuery(const QString &resourceType, const QString &resourceName)
{
    Q_D(cPCBInterface);
    return d->resourceAliasQuery(resourceType, resourceName);
}

}
