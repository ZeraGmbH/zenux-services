#ifndef ZDSPCLIENT_H
#define ZDSPCLIENT_H

#include "abstractfactorydevicenodedsp.h"
#include "dspapi.h"
#include "dspvarresolver.h"
#include <vtcp_peer.h>

class ZdspClient
{
public:
    ZdspClient(int dspInterruptId,
               VeinTcp::TcpPeer *veinPeer,
               const QByteArray &proxyConnectionId,
               AbstractFactoryDeviceNodeDspPtr deviceNodeFactory);
    virtual ~ZdspClient();

    QByteArray getProtobufClientId() const;

    bool setRawActualValueList(const QString& varsSemicolonSeparated);
    void setCmdListDef(const QString& cmdListDef);
    void setCmdForIrqListDef(const QString& cmdIntListDef);

    bool GenCmdLists(QString& errs, ulong userMemOffset, ulong globalstartadr);
    QList<DspCmdWithParamsRaw>& GetDspCmdList();
    QList<DspCmdWithParamsRaw>& GetDspIntCmdList();

    QString readActValues(const QString &variablesStringOnEmptyActOnly);
    DspVarResolver m_dspVarResolver;

    ulong relocalizeUserMemSectionVars(ulong startAdress, ulong globalMemStart);

    int getDspInterruptId() const;
    VeinTcp::TcpPeer* getVeinPeer() const;

    static int getInstanceCount();

private:
    VeinTcp::TcpPeer* m_veinPeer;
    QByteArray m_proxyConnectionId;
    AbstractFactoryDeviceNodeDspPtr m_deviceNodeFactory;
    int m_dspInterruptId;
    QString m_sCmdListDef; // kommando liste defintion
    QString m_sIntCmdListDef; // interrupt kommando  liste defintion

    QList<DspCmdWithParamsRaw> m_DspCmdList;
    QList<DspCmdWithParamsRaw>  m_DspIntCmdList;
    QVector<TDspVar> m_dspVarArray; // !!! we need permanent keeper of TDspVar pointer to data is used !!!
    TMemSection m_userMemSection;
    static int m_instanceCount;
};
#endif // ZDSPCLIENT_H
