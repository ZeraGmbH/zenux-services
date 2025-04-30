#include "dspinterface.h"
#include "dspinterface_p.h"

namespace Zera {

cDSPInterface::cDSPInterface() :
    d_ptr(new cDSPInterfacePrivate(this))
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


void cDSPInterface::clearCmdList()
{
    Q_D(cDSPInterface);
    d->clearCmdList();
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

cDspMeasData* cDSPInterface::getMemHandle(QString name)
{
    Q_D(cDSPInterface);
    return d->getMemHandle(name);
}

void cDSPInterface::deleteMemHandle(cDspMeasData *memhandle)
{
    Q_D(cDSPInterface);
    d->deleteMemHandle(memhandle);
}


quint32 cDSPInterface::activateInterface()
{
    Q_D(cDSPInterface);
    return d->activateInterface();
}

quint32 cDSPInterface::deactivateInterface()
{
    Q_D(cDSPInterface);
    return d->deactivateInterface();
}

quint32 cDSPInterface::deactivateAll()
{
    Q_D(cDSPInterface);
    return d->deactivateAll();
}

quint32 cDSPInterface::dataAcquisition(cDspMeasData *memgroup)
{
    Q_D(cDSPInterface);
    return d->dataAcquisition(memgroup);
}

quint32 cDSPInterface::dspMemoryRead(cDspMeasData *memgroup, DSPDATA::dType type)
{
    Q_D(cDSPInterface);
    return d->dspMemoryRead(memgroup, type);
}

quint32 cDSPInterface::dspMemoryWrite(cDspMeasData *memgroup)
{
    Q_D(cDSPInterface);
    return d->dspMemoryWrite(memgroup);
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

}
