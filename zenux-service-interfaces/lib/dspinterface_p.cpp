#include "dspinterface_p.h"
#include "dspinterface.h"
#include "variantconverter.h"
#include <netmessages.pb.h>

namespace Zera {

cDSPInterfacePrivate::cDSPInterfacePrivate(cDSPInterface *iface, int entityId) :
    q_ptr(iface),
    m_entityId(entityId)
{
}

cDSPInterfacePrivate::~cDSPInterfacePrivate()
{
    for(const auto dspMem : qAsConst(m_DspMemoryDataList))
        delete dspMem;
}

void cDSPInterfacePrivate::setClientSmart(ProxyClientPtr client)
{
    if (m_clientSmart) // we avoid multiple connections
        disconnect(m_clientSmart.get(), 0, this, 0);
    m_clientSmart = client;
    connect(m_clientSmart.get(), &Zera::ProxyClient::answerAvailable, this, &cDSPInterfacePrivate::receiveAnswer);
    connect(m_clientSmart.get(), &Zera::ProxyClient::tcpError, this, &cDSPInterfacePrivate::receiveError);
}

quint32 cDSPInterfacePrivate::scpiCommand(const QString &scpi)
{
    quint32 msgnr;
    QList<QString> sl = scpi.split(' ');
    if (sl.count() <= 1)
        msgnr = sendCommand(scpi);
    else
        msgnr = sendCommand(sl.at(0), sl.at(1));

    m_MsgNrCmdList[msgnr] = dspscpi;
    return msgnr;
}

/////////////////////////////////////////////////////////////////////////////////////////
// Hint: to find server SCPI implementation, search for last node name in 'long' comments
/////////////////////////////////////////////////////////////////////////////////////////

quint32 cDSPInterfacePrivate::setSamplingSystem(int chncount, int samp_per, int samp_mper)
{
    quint32 msgnr = sendCommand("SYST:DSP:SAMP", // long: SYSTEM:DSP:SAMPLING
                                QString("%1,%2,%3").arg(chncount).arg(samp_per).arg(samp_mper));
    m_MsgNrCmdList[msgnr] = setsamplingsystem;
    return msgnr;
}

QString cDSPInterfacePrivate::varList2String(VarListPrependOptions prependOption) const
{
    QString varList;
    QTextStream ts(&varList, QIODevice::WriteOnly);
    if(prependOption == PREPEND_ENTIY_ID_IF_SET)
        prependEntityIdIfSet(ts);
    for (int i = 0; i < m_DspMemoryDataList.count(); i++) {
        DspVarGroupClientInterface* pDspMeasData = m_DspMemoryDataList.at(i);
        ts << pDspMeasData->VarListLong(DSPDATA::userCreatableTypes);
    }
    return varList;
}

quint32 cDSPInterfacePrivate::varList2Dsp() // the complete list has several partial lists
{
    QString varList = varList2String(PREPEND_ENTIY_ID_IF_SET);
    quint32 msgnr = sendCommand("MEAS:LIST:RAVL", varList); // long: MEASURE:LIST:RAVLIST
    m_MsgNrCmdList[msgnr] = varlist2dsp;
    return msgnr;
}

quint32 cDSPInterfacePrivate::cmdList2Dsp()
{
    QString plist;
    QTextStream ts( &plist, QIODevice::WriteOnly );
    prependEntityIdIfSet(ts);
    for (auto it = m_cycCmdList.constBegin(); it != m_cycCmdList.constEnd(); ++it )
        ts << *it << ";" ;
    quint32 msgnr = sendCommand("MEAS:LIST:CYCL", plist); // long: MEASURE:LIST:CYCLIST
    m_MsgNrCmdList[msgnr] = cmdlist2dsp;
    return msgnr;
}

quint32 cDSPInterfacePrivate::intList2Dsp()
{
    QString plist;
    QTextStream ts( &plist, QIODevice::WriteOnly );
    prependEntityIdIfSet(ts);
    for (auto it = m_irqCmdList.constBegin(); it != m_irqCmdList.constEnd(); ++it )
        ts << *it << ";" ;
    quint32 msgnr = sendCommand("MEAS:LIST:INTL", plist); // long: MEASURE:LIST:INTLIST
    m_MsgNrCmdList[msgnr] = intlist2dsp;
    return msgnr;
}

int cDSPInterfacePrivate::cmdListCount()
{
    return m_cycCmdList.count();
}

int cDSPInterfacePrivate::intListCount()
{
    return m_irqCmdList.count();
}

// Not used in public interface
quint32 cDSPInterfacePrivate::triggerIntHKSK(quint32 hksk)
{
    quint32 msgnr = sendCommand("SYST:DSP:TRIG:INTL:HKSK", // long: SYSTEM:DSP:TRIGGER:INTLIST:HKSK
                                QString("%1").arg(hksk));
    m_MsgNrCmdList[msgnr] = triggerinthksk;
    return msgnr;
}

void cDSPInterfacePrivate::addCycListItem(const QString &cmd)
{
    m_cycCmdList.append(cmd);
}

void cDSPInterfacePrivate::addCycListItems(const QStringList &cmds)
{
    m_cycCmdList.append(cmds);
}

void cDSPInterfacePrivate::addIntListItem(const QString &cmd)
{
    m_irqCmdList.append(cmd);
}

DspVarGroupClientInterface* cDSPInterfacePrivate::getMemHandle(const QString &name)
{
    DspVarGroupClientInterface* pdmd = new DspVarGroupClientInterface(name);
    m_DspMemoryDataList.append(pdmd);
    return pdmd;
}

DspVarGroupClientInterface *cDSPInterfacePrivate::findMemHandle(const QString &name) const
{
    DspVarGroupClientInterface* memHandleFound = nullptr;
    for(int i=0; i<m_DspMemoryDataList.count(); ++i) {
        if(m_DspMemoryDataList[i]->getName() == name) {
            memHandleFound = m_DspMemoryDataList[i];
            break;
        }
    }
    return memHandleFound;
}

quint32 cDSPInterfacePrivate::activateInterface()
{
    quint32 msgnr = sendCommand("MEAS:LIST:SET"); // long: MEASURE:LIST:SET
    m_MsgNrCmdList[msgnr] = activateinterface;
    return msgnr;
}

quint32 cDSPInterfacePrivate::deactivateAll()
{
    quint32 msgnr = sendCommand("MEAS:LIST:CLAL"); // long: MEASURE:LIST:CLALL
    m_MsgNrCmdList[msgnr] = deactivateall;
    return msgnr;
}

quint32 cDSPInterfacePrivate::dataAcquisition(DspVarGroupClientInterface *varGroup)
{
    quint32 msgnr = sendCommand(QString("MEAS"), QString("%1").arg(varGroup->VarListShort(DSPDATA::vDspResult)));
    m_MsgNrCmdList[msgnr] = dataacquisition;
    m_MsgNrMeasData[msgnr] = varGroup;
    m_MsgNrMemType[msgnr] = dspDataTypeFloat;
    return msgnr;
}

quint32 cDSPInterfacePrivate::dspMemoryRead(DspVarGroupClientInterface *varGroup, DspDataType type)
{
    quint32 msgnr = sendCommand(QString("MEM:READ"), // long: MEMORY:READ
                                QString("%1").arg(varGroup->VarListShort(DSPDATA::vDspALL)));
    m_MsgNrCmdList[msgnr] = dspmemoryread;
    m_MsgNrMeasData[msgnr] = varGroup;
    m_MsgNrMemType[msgnr] = type;
    return msgnr;
}

quint32 cDSPInterfacePrivate::dspMemoryWrite(DspVarGroupClientInterface *varGroup)
{
    quint32 msgnr = sendCommand("MEM:WRIT", // long: MEMORY:WRITE
                                varGroup->writeCommand());
    m_MsgNrCmdList[msgnr] = dspmemorywrite;
    return msgnr;
}

QStringList cDSPInterfacePrivate::getCyclicCmdList() const
{
    return m_cycCmdList;
}

QList<DspVarGroupClientInterface *> cDSPInterfacePrivate::getMemoryDataList() const
{
    return m_DspMemoryDataList;
}

quint32 cDSPInterfacePrivate::readDeviceVersion()
{
    quint32 msgnr = sendCommand("SYST:VERS:DEV?"); // long: SYSTEM:VERSION:DEVICE
    m_MsgNrCmdList[msgnr] = readdeviceversion;
    return msgnr;
}

quint32 cDSPInterfacePrivate::readServerVersion()
{
    quint32 msgnr = sendCommand("SYST:VERS:SERV?"); // long: SYSTEM:VERSION:SERVER
    m_MsgNrCmdList[msgnr] = readserverversion;
    return msgnr;
}

void cDSPInterfacePrivate::receiveAnswer(std::shared_ptr<ProtobufMessage::NetMessage> message)
{
    if (message->has_reply()) {
        QString lmsg = "";
        if (message->reply().has_body())
            lmsg = QString::fromStdString(message->reply().body());

        quint8 lreply = message->reply().rtype();
        int lastCmd;
        quint32 lmsgnr = message->messagenr();
        if (lmsgnr == 0)
            lastCmd = dspinterrupt;
        else
            lastCmd = m_MsgNrCmdList.take(lmsgnr);

        Q_Q(cDSPInterface);

        switch (lastCmd)
        {
        case setsamplingsystem:
        case varlist2dsp:
        case cmdlist2dsp:
        case intlist2dsp:
        case triggerinthksk:
        case activateinterface:
        case deactivateall:
        case dspmemorywrite:
        case dspscpi:
            emit q->serverAnswer(lmsgnr, lreply, VariantConverter::returnString(lmsg));
            break;
        case readdeviceversion:
        case readserverversion:
        case dspinterrupt:
            emit q->serverAnswer(lmsgnr, lreply, VariantConverter::returnString(lmsg));
            break;
        case dataacquisition:
        case dspmemoryread: {
            DspVarGroupClientInterface* actMemGroup = m_MsgNrMeasData.take(lmsgnr);
            DspDataType actMemType = m_MsgNrMemType.take(lmsgnr);
            Q_UNUSED(actMemType)
            actMemGroup->setVarData(lmsg);
            emit q->serverAnswer(lmsgnr, 0, VariantConverter::returnString(lmsg));
            break;
        }

        }
    }
}

void cDSPInterfacePrivate::receiveError(QAbstractSocket::SocketError errorCode)
{
    Q_Q(cDSPInterface);
    emit q->tcpError(errorCode);
}

void cDSPInterfacePrivate::prependEntityIdIfSet(QTextStream &stream) const
{
    if (m_entityId >= 0)
        stream << QString("#ENTID#%1;").arg(m_entityId);
}

}
