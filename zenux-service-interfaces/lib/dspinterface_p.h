#ifndef DSPINTERFACE_P_H
#define DSPINTERFACE_P_H

#include <QObject>
#include <QList>
#include <QStringList>

#include "interface_p.h"
#include "dspinterface.h"

namespace Zera {

enum dspcommands
{
    bootdsp,
    resetdsp,
    setbootpath,
    setsamplingsystem,
    varlist2dsp,
    cmdlist2dsp,
    intlist2dsp,
    setsignalrouting,
    setdsp61850sourceadr,
    setdsp61850destadr,
    setdsp61850prioritytagged,
    setdsp61850ethtypeappid,
    setdsp61850ethsynchronisation,
    resetmaximum,
    triggerinthksk,
    activateinterface,
    deactivateinterface,
    dataacquisition,
    dspmemoryread,
    dspmemorywrite,
    setgaincorrection,
    setphasecorrection,
    setoffsetcorrection,
    readdeviceversion,
    readserverversion,
    dspinterrupt,
    readactualload,
    readmaxload,
    resetmaxload
};


class cDSPInterface;

class cDSPInterfacePrivate: public cInterfacePrivate
{
    Q_OBJECT
public:
    cDSPInterfacePrivate(cDSPInterface* iface);
    virtual ~cDSPInterfacePrivate();
    void setClient(Zera::ProxyClient *client);
    void setClientSmart(Zera::ProxyClientPtr client);
    quint32 setSamplingSystem(int chncount, int samp_per, int samp_mper); // nmuber of channels, samples/signalperiod, samples/measperiod
    quint32 varList2Dsp(); // send the var-list to dsp server
    quint32 cmdList2Dsp(); // send cyclic command list to the dsp server
    int cmdListCount(); // returns the number of command in cyclist program list
    void clearCmdList(); // clears all cmd lists
    void addCycListItem(QString cmd); // appends new command to cyclic list
    void addCycListItems(const QStringList &cmds); // appends new commands to cyclic list
    cDspMeasData* getMemHandle(QString name); // init a new memory group and return handle
    void deleteMemHandle(cDspMeasData* memhandle);
    quint32 activateInterface(); // load var- and cmdlists to dsp (starts theprogram on dsp)
    quint32 deactivateInterface(); // unload ...
    quint32 dataAcquisition(cDspMeasData* memgroup); // reads all vars of this memorygroup that are of type vapplication
    quint32 dspMemoryRead(cDspMeasData* memgroup, DSPDATA::dType type = DSPDATA::dFloat); // reads all vars of this memorygroup
    quint32 dspMemoryWrite(cDspMeasData* memgroup); // writes all vars of this memorygroup with type
    quint32 readDeviceVersion(); // reads the dsp devices version
    quint32 readServerVersion();
    quint32 readMaximumLoad();
    quint32 resetMaximumLoad();

    // unused in ZENUX - kept for sake of future WM???
    quint32 intList2Dsp(); // send interrupt command list to the dsp server
    quint32 bootDsp();
    quint32 resetDsp();
    int intListCount(); // returns the number of command in intlist program list
    void clearMemLists(); // clears all memory lists (memhandles)
    quint32 setSignalRouting(tRouting* routingtab); // set signal routing
    quint32 setDsp61850PriorityTagged(quint32 priotag); // set entries for eth frame decoder
    quint32 setDsp61850EthTypeAppId(quint32 typAppid); // same
    quint32 setDsp61850EthSynchronisation(quint32 syncdata); // setting strong sync. and dataset
    quint32 resetMaximum(); // clear maximum memory
    quint32 triggerIntHKSK(quint32 hksk); // trigger start hksk in intlist
    void addIntListItem(QString cmd); // same for interrupt list
    quint32 setGainCorrection(int chn, float val); // sets gaincorrection for 1 channel
    quint32 setPhaseCorrection(int chn, float val); // sets phase correction for 1 channel
    quint32 setOffsetCorrection(int chn, float val); // sets offset correction for 1 channel

    QStringList getCyclicCmdList() const;
protected slots:
    void receiveAnswer(std::shared_ptr<ProtobufMessage::NetMessage> message) override;
    void receiveError(QAbstractSocket::SocketError errorCode) override;

private:
    Q_DECLARE_PUBLIC(cDSPInterface)
    cDSPInterface *q_ptr;
    Zera::ProxyClientPtr m_clientSmart;

    QStringList CycCmdList, IntCmdList;
    QList<cDspMeasData*> m_DspMemoryDataList; // eine liste mit zeigern auf dsp speicher
    QHash<quint32, cDspMeasData*> m_MsgNrMeasData;
    QHash<quint32, DSPDATA::dType> m_MsgNrMemType;
};

}

#endif // DSPINTERFACE_P_H
