#ifndef ZDSPCLIENT_H
#define ZDSPCLIENT_H

#include "abstractfactoryzdspsupport.h"
#include "dspapi.h"
#include "dspvarresolver.h"
#include <vtcp_peer.h>

class ZdspClient
{
public:
    ZdspClient(int dspInterruptId,
               VeinTcp::TcpPeer *veinPeer,
               const QByteArray &proxyConnectionId,
               AbstractFactoryZdspSupportPtr zdspSupportFactory);
    virtual ~ZdspClient();

    const QByteArray &getProtobufClientId() const;

    bool setVarList(const QString& varsSemicolonSeparated);
    static void resetGlobalVarList();
    void setCmdListDef(const QString& cmdListDef);
    void setCmdForIrqListDef(const QString& cmdIntListDef);

    bool compileCmdLists(QString& errs, ulong userMemOffset, ulong alignedStartAdr);
    const QList<DspCmdWithParamsCompiled>& GetDspCmdList() const;
    const QList<DspCmdWithParamsCompiled>& GetDspIntCmdList() const;
    const DspMemorySectionInternal &getUserMemSection() const;

    DspVarResolver m_dspVarResolver;

    struct MemSizes {
        ulong usermemsize = 0;
        ulong alignedMemSize = 0;
    };
    MemSizes calcVarAdressesAndSizes(ulong startAdress, ulong alignedMemStartAddress);

    int getDspInterruptId() const;
    int getMemSize(DspSegmentType segment) const;
    static int getGlobalMemSizeTotal();
    bool hasCyclicCmds() const;
    bool hasInterruptCmds() const;
    VeinTcp::TcpPeer* getVeinPeer() const;

    // dump / tests
    void setEntityId(int entityId);
    int getEntityId() const;
    AbstractDspCompilerSupportPtr getCurrCyclicCommandsCompilerSupport() const;
    AbstractDspCompilerSupportPtr getCurrInterruptCommandsCompilerSupport() const;
    static int getInstanceCount();

private:
    VeinTcp::TcpPeer* m_veinPeer;
    QByteArray m_proxyConnectionId;
    AbstractFactoryZdspSupportPtr m_zdspSupportFactory;
    int m_dspInterruptId;

    int m_entityId = -1;
    QString m_sCmdListDef; // kommando liste defintion
    QString m_sIntCmdListDef; // interrupt kommando  liste defintion

    QList<DspCmdWithParamsCompiled> m_DspCmdList;
    QList<DspCmdWithParamsCompiled> m_DspIntCmdList;
    DspMemorySectionInternal m_userMemSection;
    static QHash<QString /* varName */, DspVarServerPtr> m_globalVariables;
    static ulong m_globalVarSize;

    // dump / tests
    AbstractDspCompilerSupportPtr m_cyclicCommandsCompilerSupport;
    AbstractDspCompilerSupportPtr m_interruptCommandsCompilerSupport;
    static int m_instanceCount;
};
#endif // ZDSPCLIENT_H
