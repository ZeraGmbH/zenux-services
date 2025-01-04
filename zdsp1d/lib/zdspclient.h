#ifndef ZDSPCLIENT_H
#define ZDSPCLIENT_H

#include "abstractfactorydevicenodedsp.h"
#include "dsp.h"
#include "dspvarresolver.h"
#include "dspvarclientperspective.h"
#include <vtcp_peer.h>

class ZDspServer;

class cZDSP1Client
{
public:
    cZDSP1Client(int socket, VeinTcp::TcpPeer *netclient, AbstractFactoryDeviceNodeDspPtr deviceNodeFactory);
    ~cZDSP1Client() = default;

    bool isActive();
    void setActive(bool active);

    bool setRawActualValueList(const QString& varsSemicolonSeparated);
    void setCmdListDef(const QString& cmdListDef);
    void setCmdForIrqListDef(const QString& cmdIntListDef);

    bool GenCmdLists(QString& errs, ulong userMemOffset, ulong globalstartadr);
    QList<DspCmdWithParamsRaw>& GetDspCmdList();
    QList<DspCmdWithParamsRaw>& GetDspIntCmdList();

    QString readDspVarList(const QString &variablesString); // format: '<name1>,<len1>;<name2>,<len2>'
    QString readActValues(const QString &variablesStringOnEmptyActOnly);
    bool readOneDspVarInt(const QString &varName, int& intval);
    TDspVar *readOneDspVar(const QString &nameCommaLen, QByteArray *varRead);
    DspVarResolver m_dspVarResolver;

    ulong setStartAdr(ulong startAdress, ulong globalMemStart); // zum relokalisieren der userdaten

    int getSocket() const;
    VeinTcp::TcpPeer* m_pNetClient;

private:
    AbstractFactoryDeviceNodeDspPtr m_deviceNodeFactory;
    int m_socket; // socket für den die verbindung besteht
    bool m_bActive = false;
    QString m_sCmdListDef; // kommando liste defintion
    QString m_sIntCmdListDef; // interrupt kommando  liste defintion

    QList<DspVarClientPerspective> m_dspRawActualValueList; // liste mit variablen beschreibungen
    QList<DspCmdWithParamsRaw> m_DspCmdList;
    QList<DspCmdWithParamsRaw>  m_DspIntCmdList;
    QVector<TDspVar> m_dspVarArray; // !!! we need permanent keeper of TDspVar pointer to data is used !!!
    TMemSection m_userMemSection;
};
#endif // ZDSPCLIENT_H
