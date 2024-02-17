#ifndef DSPINTERFACE_H
#define DSPINTERFACE_H

#include "abstractserverInterface.h"
#include "service-interfaces_export.h"
#include "dspmeasdata.h"
#include <proxyclient.h>
#include <QObject>
#include <QAbstractSocket>
#include <QVariant>

namespace Zera {

typedef quint32 tRouting[16];
class cDSPInterfacePrivate;

class SERVICE_INTERFACES_EXPORT cDSPInterface: public AbstractServerInterface
{
    Q_OBJECT
public:
    cDSPInterface();
    virtual ~cDSPInterface();
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
    void setVarData(cDspMeasData* memgroup, QString datalist, DSPDATA::dType type = DSPDATA::dFloat); //set the values of memgroup from qstring
    quint32 dspMemoryWrite(cDspMeasData* memgroup); // writes all vars of this memorygroup with type
    void getData(cDspMeasData* memgroup, QVector<float>& vector); // copy the memgroups data into qvector
    quint32 readDeviceVersion(); // reads the dsp devices version
    quint32 readServerVersion();

    // Nightmare candidates: Make them go!!!
    float* data(cDspMeasData* memgroup, QString name); // returns a pointer to the vars read from dsp

private:
    Q_DECLARE_PRIVATE(cDSPInterface)
    cDSPInterfacePrivate *d_ptr;
};

}

#endif // DSPINTERFACE_H
