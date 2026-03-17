#ifndef ZDSPCLIENT_H
#define ZDSPCLIENT_H

#include "abstractfactoryzdspsupport.h"
#include "dspapi.h"
#include "dspvarresolver.h"
#include <vtcp_peer.h>
#include <xiqnetwrapper.h>

class ZdspClient : public QObject
{
    Q_OBJECT
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
    void sendInterruptNotification(ulong parameter, XiQNetWrapper &protobufWrapper) const;

    bool compileCmdLists(QString& errs);
    const QList<DspCmdWithParamsCompiled>& GetDspCmdList() const;
    const QList<DspCmdWithParamsCompiled>& GetDspIntCmdList() const;
    const DspMemorySectionInternal &getUserMemSection() const;

    DspVarResolver m_dspVarResolver;

    void calcVarAdressesAndSizes(ulong &currLocalAbsAdr,
                                 ulong &currAlignedAbsAdr,
                                 ulong &currGlobalAbsAdr);

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
