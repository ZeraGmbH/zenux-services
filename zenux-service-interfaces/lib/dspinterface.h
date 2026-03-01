#ifndef DSPINTERFACE_H
#define DSPINTERFACE_H

#include "abstractserverInterface.h"
#include "dspvargroupclientinterface.h"
#include <proxyclient.h>
#include <QObject>
#include <QAbstractSocket>
#include <QVariant>

class DspBuffLen {
public:
    static int avgFilterLen(int entries) { return 2*entries+1; /* avg double precision + 1 counter */}
    static int avgFilterCountPos(int entries) { return 2*entries; /* avg double precision + 1 counter */}
};

namespace Zera {

class cDSPInterfacePrivate;

class cDSPInterface: public AbstractServerInterface
{
    Q_OBJECT
public:
    cDSPInterface(int entityId = -1);
    virtual ~cDSPInterface();
    void setClientSmart(Zera::ProxyClientPtr client);
    quint32 scpiCommand(const QString &scpi) override;

    virtual quint32 dataAcquisition(DspVarGroupClientInterface* varGroup); // reads all vars of this variable group
    virtual quint32 dspMemoryWrite(DspVarGroupClientInterface* varGroup); // writes all vars of this variable group
    virtual quint32 activateInterface(); // load var- and cmdlists to dsp (starts theprogram on dsp)

    quint32 setSamplingSystem(int chncount, int samp_per, int samp_mper); // nmuber of channels, samples/signalperiod, samples/measperiod

    DspVarGroupClientInterface* createVariableGroup(const QString &varGroupName);
    quint32 varList2Dsp(); // send the var-list to dsp server

    int cmdListCount(); // returns the number of command in cyclist program list
    void addCycListItem(QString cmd); // appends new command to cyclic list
    void addCycListItems(const QStringList &cmds); // appends new commands to cyclic list
    quint32 cmdList2Dsp(); // send cyclic command list to the dsp server

    quint32 deactivateAll();

    // TODO!!! what is the difference to dataAcquisition??
    quint32 dspMemoryRead(DspVarGroupClientInterface* varGroup);
    quint32 readDeviceVersion();
    quint32 readServerVersion();

    // Compromise for tests
    QString varList2String();

protected:
    Q_DECLARE_PRIVATE(cDSPInterface)
    cDSPInterfacePrivate *d_ptr;
};

typedef std::shared_ptr<cDSPInterface> DspInterfacePtr;

}

#endif // DSPINTERFACE_H
