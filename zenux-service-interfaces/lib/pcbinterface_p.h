#ifndef PCBINTERFACE_P_H
#define PCBINTERFACE_P_H

#include <QObject>

#include "interface_p.h"

namespace Zera { namespace PCB {
enum pcbcommands
{
    getchannellist,
    getdspchannel,
    getstatus,
    resetstatus,
    getalias,
    gettype,
    getunit,
    getrange,
    getrangelist,
    getalias2,
    gettype2,
    geturvalue,
    geturvalue2,
    getrejection,
    getovrejection,
    getadcrejection,
    isavail,
    getgaincorrection,
    getoffsetcorrection,
    getphasecorrection,
    setrange,
    setmeasuringmode,
    adjustcomputation,
    adjuststorage,
    adjuststorageclamp,
    adjustinit,
    setadjustgainstatus,
    setadjustphasestatus,
    setadjustoffsetstatus,
    setgainnode,
    setphasenode,
    setoffsetnode,

    regnotifier,
    unregnotifier,
    pcbinterrupt,
    getsamplerate,
    setpllchannel,

    getaliassource,
    getdspchannelsource,
    getadjustmentstatus,
    getauthorizationstatus,
    getpcberrorstatus,
    getadjustmentchksum,
    getadjustpcbxml,
    setadjustpcbxml,
    getadjustclampxml,
    setadjustclampxml,
    getformfactorsource,
    getconstantsource,
    setconstantsource,
    getpowtypesource,
    setpowtypesource,

    getaliassschead,
    getaliasfrqinput,
    resourcealiasquery,

    getserverversion,
    getpcbversion,
    getfpgaversion,
    getctrlversion,
    getserialnumber,
    setserialnumber,

    getaccumulatorstatus,
    getaccumulatorsoc,

    transparentcommand
};
}

class cPCBInterface;

class cPCBInterfacePrivate: public cInterfacePrivate
{
    Q_OBJECT

public:
    cPCBInterfacePrivate(cPCBInterface* iface);
    void setClient(Zera::ProxyClient *client);
    void setClientSmart(Zera::ProxyClientPtr client);
    quint32 getChannelList(); // qstringlist
    quint32 getDSPChannel(QString chnName); // int
    quint32 getStatus(QString chnName); // int
    quint32 resetStatus(QString chnName); // reply (ack, nak..)
    quint32 getAlias(QString chnName); // qstring
    quint32 getType(QString chnName); // qstring
    quint32 getUnit(QString chnName); // qstring
    quint32 getRange(QString chnName); // qstring
    quint32 getRangeList(QString chnName); // qstringlist
    quint32 getAlias(QString chnName, QString rngName); // qstring
    quint32 getType(QString chnName, QString rngName); // int
    quint32 getUrvalue(QString chnName, QString rngName); // double
    quint32 getUrvalue(QString chnName); // double
    quint32 getRejection(QString chnName, QString rngName); // double
    quint32 getOVRejection(QString chnName, QString rngName); // double
    quint32 getADCRejection(QString chnName, QString rngName); // double
    quint32 isAvail(QString chnName, QString rngName); // bool
    quint32 getGainCorrection(QString chnName, QString rngName, double at); // double
    quint32 getAdjGainCorrection(QString chnName, QString rngName, double at); // double
    quint32 getOffsetCorrection(QString chnName, QString rngName, double at); // double
    quint32 getAdjOffsetCorrection(QString chnName, QString rngName, double at); // double
    quint32 getPhaseCorrection(QString chnName, QString rngName, double at); // double
    quint32 getAdjPhaseCorrection(QString chnName, QString rngName, double at); // double
    quint32 setRange(QString chnName, QString rngName); // reply (ack, nak..)
    quint32 setMMode(QString mmode);
    quint32 adjustComputation(); // all correction coefficients will be computed
    quint32 adjustStorage(); // all correction data will be stored
    quint32 adjustStorageClamp(); // all clamp correction data will be stored
    quint32 setAdjustGainStatus(QString chnName, QString rngName, int stat);
    quint32 setAdjustPhaseStatus(QString chnName, QString rngName, int stat);
    quint32 setAdjustOffsetStatus(QString chnName, QString rngName, int stat);
    quint32 setGainNode(QString chnName, QString rngName, int nr, double corr, double at); // node nr, correction, loadpoint
    quint32 setPhaseNode(QString chnName, QString rngName, int nr, double corr, double at); // node nr, correction, loadpoint = frequency
    quint32 setOffsetNode(QString chnName, QString rngName, int nr, double corr, double at); // node nr, correction, loadpoint

    // all commands to source interface
    quint32 getAliasSource(QString chnName); // qstring
    quint32 getDSPChannelSource(QString chnName); // int
    quint32 getFormFactorSource(QString chnName); // double
    quint32 getConstantSource(QString chnName); // double
    quint32 setConstantSource(QString chnName, double constant); // double
    quint32 getPowTypeSource(QString chnName); // qstring
    quint32 setPowTypeSource(QString chnName, QString ptype);

    // all commands to sample interface
    quint32 getSampleRate(); // int the actual set sampling rate
    quint32 setPLLChannel(QString samplechnName, QString pllchnName);

    // all commands to schead interface
    quint32 getAliasSchead(QString chnName); // qstring

    // all commands to frqinput interface
    quint32 getAliasFrqinput(QString chnName); // qstring

    // all commands to status interface
    quint32 getAdjustmentStatus(); // int
    quint32 getAuthorizationStatus();

    // resource query more generell
    quint32 resourceAliasQuery(QString resourceType, QString resourceName);

    quint32 registerNotifier(QString query, int notifyId, bool notifyWithValue); // register for notification on change
    quint32 unregisterNotifiers(); // unregister from all notifications

    // all commands to system interface
    quint32 readServerVersion();
    quint32 readPCBInfo();
    quint32 readFPGAVersion();
    quint32 readCTRLVersion();
    quint32 readSerialNr();
    quint32 writeSerialNr(QString serNr);
    quint32 getAdjustmentChksum();
    quint32 getPCBAdjustmentData();
    quint32 setPCBAdjustmentData(QString xmlpcb);
    quint32 getClampAdjustmentData();
    quint32 setClampAdjustmentData(QString xmlclamp);

    // all commands to accumulator interface
    quint32 getAccumulatorStatus();
    quint32 getAccuStateOfCharge();

    // sending a transparent command
    quint32 transparentCommand(QString cmd);

protected slots:
    void receiveAnswer(std::shared_ptr<ProtobufMessage::NetMessage> message) override;
    void receiveError(QAbstractSocket::SocketError errorCode) override;

private:
    Q_DECLARE_PUBLIC(cPCBInterface)
    cPCBInterface *q_ptr;
    Zera::ProxyClientPtr m_clientSmart;
};

}

#endif // PCBINTERFACE_P_H
