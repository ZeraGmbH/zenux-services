#ifndef DSPINTERFACE_H
#define DSPINTERFACE_H

#include "abstractserverInterface.h"
#include "dspmeasdata.h"
#include <proxyclient.h>
#include <QObject>
#include <QAbstractSocket>
#include <QVariant>

namespace Zera {

class cDSPInterfacePrivate;

class cDSPInterface: public AbstractServerInterface
{
    Q_OBJECT
public:
    cDSPInterface();
    virtual ~cDSPInterface();
    void setClientSmart(Zera::ProxyClientPtr client);
    quint32 scpiCommand(const QString &scpi) override;

    virtual quint32 dataAcquisition(cDspMeasData* memgroup); // reads all vars of this memorygroup that are of type vapplication
    virtual quint32 dspMemoryWrite(cDspMeasData* memgroup); // writes all vars of this memorygroup with type
    virtual quint32 activateInterface(); // load var- and cmdlists to dsp (starts theprogram on dsp)

    quint32 setSamplingSystem(int chncount, int samp_per, int samp_mper); // nmuber of channels, samples/signalperiod, samples/measperiod
    quint32 varList2Dsp(); // send the var-list to dsp server
    quint32 cmdList2Dsp(); // send cyclic command list to the dsp server
    int cmdListCount(); // returns the number of command in cyclist program list
    void clearCmdList(); // clears all cmd lists
    void addCycListItem(QString cmd); // appends new command to cyclic list
    void addCycListItems(const QStringList &cmds); // appends new commands to cyclic list
    cDspMeasData* getMemHandle(QString name); // init a new memory group and return handle
    void deleteMemHandle(cDspMeasData* memhandle);

    quint32 deactivateInterface(); // unload ...
    quint32 deactivateAll();

    // TODO!!! make DSPDATA::dType type go as we did on dspMemoryWrite
    quint32 dspMemoryRead(cDspMeasData* memgroup, DSPDATA::dType type = DSPDATA::dFloat); // reads all vars of this memorygroup
    quint32 readDeviceVersion(); // reads the dsp devices version
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
