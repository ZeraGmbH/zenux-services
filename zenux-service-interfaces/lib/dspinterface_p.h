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
    dspscpi,
    setsamplingsystem,
    varlist2dsp,
    cmdlist2dsp,
    intlist2dsp,
    triggerinthksk,
    activateinterface,
    deactivateinterface,
    deactivateall,
    dataacquisition,
    dspmemoryread,
    dspmemorywrite,
    readdeviceversion,
    readserverversion,
    dspinterrupt,
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
    quint32 scpiCommand(const QString &scpi);

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
    quint32 deactivateAll();

    quint32 dataAcquisition(cDspMeasData* memgroup); // reads all vars of this memorygroup that are of type vapplication
    quint32 dspMemoryRead(cDspMeasData* memgroup, DSPDATA::dType type = DSPDATA::dFloat); // reads all vars of this memorygroup
    quint32 dspMemoryWrite(cDspMeasData* memgroup); // writes all vars of this memorygroup with type
    quint32 readDeviceVersion(); // reads the dsp devices version
    quint32 readServerVersion();

    // unused in ZENUX - kept for sake of future WM???
    void addIntListItem(QString cmd); // same for interrupt list
    quint32 intList2Dsp(); // send interrupt command list to the dsp server
    int intListCount(); // returns the number of command in intlist program list
    quint32 triggerIntHKSK(quint32 hksk); // trigger start hksk in intlist

    // Insights for tests - a compromise...
    QStringList getCyclicCmdList() const;
    QList<cDspMeasData*> getMemoryDataList() const;
    QString varList2String() const;
    cDspMeasData* findMemHandle(QString name) const;
protected slots:
    void receiveAnswer(std::shared_ptr<ProtobufMessage::NetMessage> message) override;
    void receiveError(QAbstractSocket::SocketError errorCode) override;

private:
    Q_DECLARE_PUBLIC(cDSPInterface)
    cDSPInterface *q_ptr;
    Zera::ProxyClientPtr m_clientSmart;

    QStringList m_cycCmdList, m_irqCmdList;
    QList<cDspMeasData*> m_DspMemoryDataList; // eine liste mit zeigern auf dsp speicher
    QHash<quint32, cDspMeasData*> m_MsgNrMeasData;
    QHash<quint32, DSPDATA::dType> m_MsgNrMemType;
};

}

#endif // DSPINTERFACE_P_H
