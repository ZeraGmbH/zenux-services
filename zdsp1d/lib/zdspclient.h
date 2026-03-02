#ifndef ZDSPCLIENT_H
#define ZDSPCLIENT_H

#include "abstractfactoryzdspsupport.h"
#include "dspapi.h"
#include "dspvarresolver.h"
#include <vtcp_peer.h>
#include <memory>

class ZdspClient
{
public:
    ZdspClient(int dspInterruptId,
               VeinTcp::TcpPeer *veinPeer,
               const QByteArray &proxyConnectionId,
               AbstractFactoryZdspSupportPtr zdspSupportFactory);
    virtual ~ZdspClient();

    QByteArray getProtobufClientId() const;

    bool setVarList(const QString& varsSemicolonSeparated);
    void setCmdListDef(const QString& cmdListDef);
    void setCmdForIrqListDef(const QString& cmdIntListDef);

    bool GenCmdLists(QString& errs, ulong userMemOffset, ulong globalstartadr);
    const QList<DspCmdWithParamsRaw>& GetDspCmdList() const;
    const QList<DspCmdWithParamsRaw>& GetDspIntCmdList() const;

    DspVarResolver m_dspVarResolver;

    ulong relocalizeUserMemSectionVars(ulong startAdress, ulong globalMemStart);

    int getDspInterruptId() const;
    int getDataMemSize() const;
    VeinTcp::TcpPeer* getVeinPeer() const;

    // dump / tests
    int getEntityId() const;
    struct VarLocation
    {
        QString m_variableName;
        ulong m_localVariableAddress;
        ulong m_absoluteVariableAddress;
    };
    const QList<VarLocation>* getLocalVariableDump() const;
    const QList<VarLocation>* getGlobalVariableDump() const;
    const QStringList& getDspCmdListRaw() const;
    const QStringList& getDspIntCmdListRaw() const;
    quint32 getDspCmdListCompiledCrc() const;
    quint32 getDspIntCmdCompiledCrc() const;
    static int getInstanceCount();

private:
    int calcDataMemSize();
    void setEntityId(int entityId);
    QString handleAndRemoveEntityId(const QString &scpiParam);

    VeinTcp::TcpPeer* m_veinPeer;
    QByteArray m_proxyConnectionId;
    AbstractFactoryZdspSupportPtr m_zdspSupportFactory;
    int m_dspInterruptId;

    int m_entityId = -1;
    QString m_sCmdListDef; // kommando liste defintion
    QString m_sIntCmdListDef; // interrupt kommando  liste defintion

    QList<DspCmdWithParamsRaw> m_DspCmdList;
    QList<DspCmdWithParamsRaw>  m_DspIntCmdList;
    DspMemorySectionInternal m_userMemSection;
    int m_dataMemSize = 0;

    // dump / tests
    AbstractDspCompilerSupportPtr m_rawCyclicCommands;
    AbstractDspCompilerSupportPtr m_rawInterruptCommands;
    std::unique_ptr<QList<VarLocation>> m_localVarDump;
    std::unique_ptr<QList<VarLocation>> m_globalVarDump;
    static int m_instanceCount;
};
#endif // ZDSPCLIENT_H
