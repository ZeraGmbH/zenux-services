#include "dspinterface.h"
#include "dspinterface_p.h"

namespace Zera {

cDSPInterface::cDSPInterface(int entityId) :
    d_ptr(new cDSPInterfacePrivate(this, entityId))
{
}

cDSPInterface::~cDSPInterface()
{
    delete d_ptr;
}

void cDSPInterface::setClientSmart(ProxyClientPtr client)
{
    Q_D(cDSPInterface);
    return d->setClientSmart(client);
}

quint32 cDSPInterface::scpiCommand(const QString &scpi)
{
    Q_D(cDSPInterface);
    return d->scpiCommand(scpi);
}

quint32 cDSPInterface::setSamplingSystem(int chncount, int samp_per, int samp_mper)
{
    Q_D(cDSPInterface);
    return d->setSamplingSystem(chncount, samp_per, samp_mper);
}


quint32 cDSPInterface::varList2Dsp()
{
    Q_D(cDSPInterface);
    return d->varList2Dsp();
}

quint32 cDSPInterface::cmdList2Dsp()
{
    Q_D(cDSPInterface);
    return d->cmdList2Dsp();
}

int cDSPInterface::cmdListCount()
{
    Q_D(cDSPInterface);
    return d->cmdListCount();
}

void cDSPInterface::addCycListItem(QString cmd)
{
    Q_D(cDSPInterface);
    d->addCycListItem(cmd);
}

void cDSPInterface::addCycListItems(const QStringList &cmds)
{
    Q_D(cDSPInterface);
    d->addCycListItems(cmds);
}

DspVarGroupClientInterface* cDSPInterface::createVariableGroup(const QString &varGroupName)
{
    Q_D(cDSPInterface);
    return d->createVariableGroup(varGroupName);
}

quint32 cDSPInterface::activateInterface()
{
    Q_D(cDSPInterface);
    return d->activateInterface();
}

quint32 cDSPInterface::deactivateAll()
{
    Q_D(cDSPInterface);
    return d->deactivateAll();
}

quint32 cDSPInterface::dataAcquisition(DspVarGroupClientInterface *varGroup)
{
    Q_D(cDSPInterface);
    return d->dataAcquisition(varGroup);
}

quint32 cDSPInterface::dspMemoryRead(DspVarGroupClientInterface *varGroup)
{
    Q_D(cDSPInterface);
    return d->dspMemoryRead(varGroup);
}

quint32 cDSPInterface::dspMemoryWrite(DspVarGroupClientInterface *varGroup)
{
    Q_D(cDSPInterface);
    return d->dspMemoryWrite(varGroup);
}

quint32 cDSPInterface::readDeviceVersion()
{
    Q_D(cDSPInterface);
    return d->readDeviceVersion();
}

quint32 cDSPInterface::readServerVersion()
{
    Q_D(cDSPInterface);
    return d->readServerVersion();
}

QString cDSPInterface::varList2String()
{
    Q_D(cDSPInterface);
    return d->varList2String(cDSPInterfacePrivate::PREPEND_ENTIY_ID_IF_SET);
}

}
