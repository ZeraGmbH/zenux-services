#ifndef DSPINTERFACE_H
#define DSPINTERFACE_H

#include "abstractserverInterface.h"
#include "dspmeasdata.h"
#include <proxyclient.h>
#include <QObject>
#include <QAbstractSocket>
#include <QVariant>

namespace Zera {

typedef quint32 tRouting[16];
class cDSPInterfacePrivate;

class cDSPInterface: public AbstractServerInterface
{
    Q_OBJECT
public:
    cDSPInterface();
    virtual ~cDSPInterface();
    void setClient(Zera::ProxyClient *client);
    void setClientSmart(Zera::ProxyClientPtr client);

    virtual quint32 dataAcquisition(cDspMeasData* memgroup); // reads all vars of this memorygroup that are of type vapplication
    virtual quint32 dspMemoryWrite(cDspMeasData* memgroup); // writes all vars of this memorygroup with type

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
    quint32 dspMemoryRead(cDspMeasData* memgroup, DSPDATA::dType type = DSPDATA::dFloat); // reads all vars of this memorygroup
    quint32 readDeviceVersion(); // reads the dsp devices version
    quint32 readServerVersion();

private:
    Q_DECLARE_PRIVATE(cDSPInterface)
    cDSPInterfacePrivate *d_ptr;
};

}

#endif // DSPINTERFACE_H
