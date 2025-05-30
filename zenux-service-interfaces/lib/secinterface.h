#ifndef SECINTERFACE_H
#define SECINTERFACE_H

#include "abstractserverInterface.h"
#include "service-interfaces_export.h"
#include <proxyclient.h>

namespace ECALCREG {
    enum { CMD, CONF, STATUS, INTMASK, INTREG, MTCNTin, MTCNTfin, MTCNTact, MTPULSin = 12, MTPAUSEin, MTPULS, MTPAUSE};
}

namespace ECALCCMDID {
    //enum { COUNTEDGE = 1, COUNTRESET, ERRORMEASMASTER, ERRORMEASSLAVE};
    enum {SINGLEERRORMASTER, CONTERRORMASTER, ERRORMEASSLAVE};
}

namespace ECALCSTATUS {
    /* Note: SEC currently uses 7 least significant bits. Since we need additional
     * bits to signal states (e.g wait), we use topmost bits for that.
     */
    enum {
        // SEC bits:
        ARMED   = (1<<0),
        STARTED = (1<<1),
        READY   = (1<<2),
        ABORT   = (1<<3),
        // module-specific bits - see note above
        WAIT    = (1<<30),
        NOOP    = (1<<31) // on 32 Bit (QML) clients the causes trouble
    };
}

namespace ECALCINT {
    enum { MTposEdge = 1, MTnegEdge = 2, MTCount0 = 4};
}

namespace ECALCRESULT {
    enum enResultTypes { RESULT_UNFINISHED = -1, RESULT_FAILED = 0, RESULT_PASSED = 1};
}


namespace Zera {
class cSECInterfacePrivate;

class SERVICE_INTERFACES_EXPORT cSECInterface: public AbstractServerInterface
{
    Q_OBJECT
public:
    cSECInterface();
    virtual ~cSECInterface();
    void setClientSmart(Zera::ProxyClientPtr client);

    quint32 scpiCommand(const QString &scpi) override;
    quint32 getChannelCatalog();
    quint32 setECalcUnit(int n); // we want n eclac units, answer is the names of them
    quint32 freeECalcUnits(); // free all the units the client had set
    quint32 writeRegister(QString chnname, quint8 reg, quint32 value); // transp. register access
    quint32 readRegister(QString chnname, quint8 reg);

    quint32 setSync(QString chnname, QString syncchn);
    quint32 setMux(QString chnname, QString inpname);
    quint32 setCmdid(QString chnname, quint8 cmdid);
    quint32 start(QString chnname);
    quint32 stop(QString chnname);
    quint32 intAck(QString chnname, quint8 interrupt);

    quint32 registerNotifier(QString query); // register for notification on change
    quint32 unregisterNotifiers(); // unregister from all notifications
    static constexpr quint32 maxSecCounterInitVal = std::numeric_limits<quint32>::max() - 1;

private:
    Q_DECLARE_PRIVATE(cSECInterface)
    cSECInterfacePrivate *d_ptr;
};

typedef std::unique_ptr<cSECInterface> cSECInterfacePtr;

}

#endif // SECINTERFACE_H
