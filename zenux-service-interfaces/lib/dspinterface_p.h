#ifndef DSPINTERFACE_P_H
#define DSPINTERFACE_P_H

#include "interface_p.h"
#include "dspinterface.h"
#include <QObject>
#include <QList>
#include <QStringList>

namespace Zera {

enum dspcommands
{
    dspscpi,
    setsamplingsystem,
    varlist2dsp,
    cmdlist2dsp,
    intlist2dsp,
    triggerinthksk,
    activateinterface,
    deactivateall,
    dspmemoryread,
    dspmemorywrite,
    readdeviceversion,
    readserverversion,
    dspinterrupt,
};

class cDSPInterfacePrivate: public cInterfacePrivate
{
    Q_OBJECT
public:
    cDSPInterfacePrivate(cDSPInterface* iface, int entityId);
    virtual ~cDSPInterfacePrivate();
    void setClientSmart(Zera::ProxyClientPtr client);
    quint32 scpiCommand(const QString &scpi);

    quint32 setSamplingSystem(int chncount, int samp_per, int samp_mper); // nmuber of channels, samples/signalperiod, samples/measperiod
    quint32 varList2Dsp(); // send the var-list to dsp server
    quint32 cmdList2Dsp(); // send cyclic command list to the dsp server
    int cmdListCount(); // returns the number of command in cyclist program list
    void addCycListItem(const QString &cmd); // appends new command to cyclic list
    void addCycListItems(const QStringList &cmds); // appends new commands to cyclic list

    DspVarGroupClientInterface* createVariableGroup(const QString &varGroupName);

    quint32 activateInterface(); // load var- and cmdlists to dsp (starts theprogram on dsp)
    quint32 deactivateAll();

    quint32 dspMemoryRead(DspVarGroupClientInterface* varGroup);
    quint32 dspMemoryWrite(DspVarGroupClientInterface* varGroup);
    quint32 readDeviceVersion();
    quint32 readServerVersion();

    // unused in ZENUX - kept for sake of future WM???
    void addIntListItem(const QString &cmd); // same for interrupt list
    quint32 intList2Dsp(); // send interrupt command list to the dsp server
    int intListCount(); // returns the number of command in intlist program list
    quint32 triggerIntHKSK(quint32 hksk); // trigger start hksk in intlist

    // Insights for tests - a compromise...
    QStringList getCyclicCmdList() const;
    QList<DspVarGroupClientInterface*> getMemoryDataList() const;
    enum VarListPrependOptions {
        PREPEND_NOTHING,
        PREPEND_ENTIY_ID_IF_SET
    };
    QString varList2String(VarListPrependOptions prependOption) const;
    DspVarGroupClientInterface* findVariableGroup(const QString &name) const;
protected slots:
    void receiveAnswer(std::shared_ptr<ProtobufMessage::NetMessage> message) override;
    void receiveError(QAbstractSocket::SocketError errorCode) override;

private:
    Q_DECLARE_PUBLIC(cDSPInterface)
    void prependEntityIdIfSet(QTextStream &stream) const;
    cDSPInterface *q_ptr;
    int m_entityId;

    QStringList m_cycCmdList, m_irqCmdList;
    QList<DspVarGroupClientInterface*> m_DspMemoryDataList; // eine liste mit zeigern auf dsp speicher
    QHash<quint32, DspVarGroupClientInterface*> m_MsgNrMeasData;
};

}

#endif // DSPINTERFACE_P_H
