#include "dspinterface_p.h"
#include "dspinterface.h"
#include "variantconverter.h"
#include <netmessages.pb.h>

namespace Zera {

cDSPInterfacePrivate::cDSPInterfacePrivate(cDSPInterface *iface) :
    q_ptr(iface)
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

QString cDSPInterfacePrivate::varList2String() const
{
    QString varList;
    QTextStream ts(&varList, QIODevice::WriteOnly);
    cDspMeasData* pDspMeasData;
    for (int i = 0; i < m_DspMemoryDataList.count(); i++) {
        pDspMeasData = m_DspMemoryDataList.at(i);
        ts << pDspMeasData->VarListLong(DSPDATA::vDspParam | DSPDATA::vDspTemp | DSPDATA::vDspResult | DSPDATA::vDspTempGlobal);
    }
    return varList;
}

quint32 cDSPInterfacePrivate::varList2Dsp() // the complete list has several partial lists
{
    QString varList = varList2String();
    quint32 msgnr = sendCommand("MEAS:LIST:RAVL", varList); // long: MEASURE:LIST:RAVLIST
    m_MsgNrCmdList[msgnr] = varlist2dsp;
    return msgnr;
}

quint32 cDSPInterfacePrivate::cmdList2Dsp()
{
    QString plist;
    QTextStream ts( &plist, QIODevice::WriteOnly );
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

void cDSPInterfacePrivate::clearCmdList()
{
    m_cycCmdList.clear();
    m_irqCmdList.clear();
}

// Not used in public interface
quint32 cDSPInterfacePrivate::triggerIntHKSK(quint32 hksk)
{
    quint32 msgnr = sendCommand("SYST:DSP:TRIG:INTL:HKSK", // long: SYSTEM:DSP:TRIGGER:INTLIST:HKSK
                                QString("%1").arg(hksk));
    m_MsgNrCmdList[msgnr] = triggerinthksk;
    return msgnr;
}

void cDSPInterfacePrivate::addCycListItem(QString cmd)
{
    m_cycCmdList.append(cmd);
}

void cDSPInterfacePrivate::addCycListItems(const QStringList &cmds)
{
    m_cycCmdList.append(cmds);
}

void cDSPInterfacePrivate::addIntListItem(QString cmd)
{
    m_irqCmdList.append(cmd);
}

cDspMeasData* cDSPInterfacePrivate::getMemHandle(QString name)
{
    cDspMeasData* pdmd = new cDspMeasData(name);
    m_DspMemoryDataList.append(pdmd);
    return pdmd;
}

cDspMeasData *cDSPInterfacePrivate::findMemHandle(QString name) const
{
    cDspMeasData* memHandleFound = nullptr;
    for(int i=0; i<m_DspMemoryDataList.count(); ++i) {
        if(m_DspMemoryDataList[i]->getName() == name) {
            memHandleFound = m_DspMemoryDataList[i];
            break;
        }
    }
    return memHandleFound;
}

void cDSPInterfacePrivate::deleteMemHandle(cDspMeasData *memhandle)
{
    if (m_DspMemoryDataList.contains(memhandle)) {
        int index = m_DspMemoryDataList.indexOf(memhandle);
        cDspMeasData* pdmd = m_DspMemoryDataList.takeAt(index);
        delete pdmd;
    }
}

quint32 cDSPInterfacePrivate::activateInterface()
{
    quint32 msgnr = sendCommand("MEAS:LIST:SET"); // long: MEASURE:LIST:SET
    m_MsgNrCmdList[msgnr] = activateinterface;
    return msgnr;
}

quint32 cDSPInterfacePrivate::deactivateInterface()
{
    quint32 msgnr = sendCommand("MEAS:LIST:CLE"); // long: MEASURE:LIST:CLEAR
    m_MsgNrCmdList[msgnr] = deactivateinterface;
    return msgnr;
}

quint32 cDSPInterfacePrivate::deactivateAll()
{
    quint32 msgnr = sendCommand("MEAS:LIST:CLAL"); // long: MEASURE:LIST:CLALL
    m_MsgNrCmdList[msgnr] = deactivateall;
    return msgnr;
}

quint32 cDSPInterfacePrivate::dataAcquisition(cDspMeasData *memgroup)
{
    quint32 msgnr = sendCommand(QString("MEAS"), QString("%1").arg(memgroup->VarListShort(DSPDATA::vDspResult)));
    m_MsgNrCmdList[msgnr] = dataacquisition;
    m_MsgNrMeasData[msgnr] = memgroup;
    m_MsgNrMemType[msgnr] = DSPDATA::dFloat;
    return msgnr;
}

quint32 cDSPInterfacePrivate::dspMemoryRead(cDspMeasData *memgroup, DSPDATA::dType type)
{
    quint32 msgnr = sendCommand(QString("MEM:READ"), // long: MEMORY:READ
                                QString("%1").arg(memgroup->VarListShort(DSPDATA::vDspALL)));
    m_MsgNrCmdList[msgnr] = dspmemoryread;
    m_MsgNrMeasData[msgnr] = memgroup;
    m_MsgNrMemType[msgnr] = type;
    return msgnr;
}

quint32 cDSPInterfacePrivate::dspMemoryWrite(cDspMeasData *memgroup)
{
    quint32 msgnr = sendCommand("MEM:WRIT", // long: MEMORY:WRITE
                                memgroup->writeCommand());
    m_MsgNrCmdList[msgnr] = dspmemorywrite;
    return msgnr;
}

QStringList cDSPInterfacePrivate::getCyclicCmdList() const
{
    return m_cycCmdList;
}

QList<cDspMeasData *> cDSPInterfacePrivate::getMemoryDataList() const
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
        quint32 lmsgnr = message->messagenr();
        QString lmsg = "";
        if (message->reply().has_body())
            lmsg = QString::fromStdString(message->reply().body());

        quint8 lreply = message->reply().rtype();
        int lastCmd;
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
        case deactivateinterface:
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
            cDspMeasData* actMemGroup = m_MsgNrMeasData.take(lmsgnr);
            DSPDATA::dType actMemType = m_MsgNrMemType.take(lmsgnr);
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

}
