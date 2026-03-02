#include "zdspclient.h"
#include "dspcmdcompiler.h"
#include "dspvardevicenodeinout.h"
#include <pseudocrcbuffer.h>
#include <QDataStream>

int ZdspClient::m_instanceCount = 0;

ZdspClient::ZdspClient(int dspInterruptId,
                       VeinTcp::TcpPeer* veinPeer,
                       const QByteArray &proxyConnectionId,
                       AbstractFactoryZdspSupportPtr zdspSupportFactory) :
    m_veinPeer(veinPeer),
    m_proxyConnectionId(proxyConnectionId),
    m_zdspSupportFactory(zdspSupportFactory),
    m_dspInterruptId(dspInterruptId),
    m_rawCyclicCommands(m_zdspSupportFactory->createDspCompilerSupport()),
    m_rawInterruptCommands(m_zdspSupportFactory->createDspCompilerSupport()),
    m_localVarDump(std::make_unique<QList<VarLocation>>()),
    m_globalVarDump(std::make_unique<QList<VarLocation>>())
{
    m_instanceCount++;
    DspCmdWithParamsRaw DspCmd;
    m_DspCmdList.append(DspCmd);
    m_DspIntCmdList.append(DspCmd);
    m_dspVarResolver.addSection(&m_userMemSection);
}

ZdspClient::~ZdspClient()
{
    m_instanceCount--;
}

void ZdspClient::setEntityId(int entityId)
{
    if (m_entityId >= 0 && entityId != m_entityId) {
        qCritical("Trying to set entity Id %i on ZdspClient which has already ID %i!",
                  entityId, m_entityId);
        return;
    }
    m_entityId = entityId;
}

int ZdspClient::getEntityId() const
{
    return m_entityId;
}

const QList<ZdspClient::VarLocation> *ZdspClient::getGlobalVariableDump() const
{
    return m_globalVarDump.get();
}

const QList<ZdspClient::VarLocation> *ZdspClient::getLocalVariableDump() const
{
    return m_localVarDump.get();
}

bool ZdspClient::setVarList(const QString &varsSemicolonSeparated)
{
    m_userMemSection.clear();
    int localOffset = 0;
    int globaloffset = 0;
    const QString varsSemicolonSeparatedEntityIdStripped = handleAndRemoveEntityId(varsSemicolonSeparated);
    const QStringList varEntries = varsSemicolonSeparatedEntityIdStripped.split(";", Qt::SkipEmptyParts);
    bool allOk = true;
    for(int i=0; i<varEntries.count(); i++) {
        DspVarServer dspVar;
        if(dspVar.setupFromCommaSeparatedString(varEntries[i])) {
            if (dspVar.segment == dspInternalSegment) {
                const DspVarServerPtr dspVarDsp = m_dspVarResolver.getDspVar(dspVar.Name);
                if (dspVarDsp == nullptr) {
                    qCritical("Internal DSP Variable %s to add on client not found",
                              qPrintable(dspVar.Name));
                    allOk = false;
                }
                else if (dspVarDsp->size != dspVar.size) {
                    qCritical("Internal DSP Variable %s to add on client has wrong size: wanted: %i / expected %i",
                              qPrintable(dspVar.Name), dspVar.size, dspVarDsp->size);
                    allOk = false;
                }
                else if (dspVarDsp->type != dspVar.type) {
                    qCritical("Internal DSP Variable %s to add on client has wrong type: wanted: %i / expected %i",
                              qPrintable(dspVar.Name), dspVar.type, dspVarDsp->type);
                    allOk = false;
                }
            }
            else if (dspVar.segment == moduleLocalSegment) {
                dspVar.offs = localOffset;
                localOffset += dspVar.size;
                m_userMemSection.appendDspVar(dspVar);
            }
            else if (dspVar.segment == moduleGlobalSegment) {
                dspVar.offs = globaloffset;
                // TODO already there / other clients????
                globaloffset += dspVar.size;
                m_userMemSection.appendDspVar(dspVar);
            }
        }
        else
            allOk = false;
    }
    if (!allOk)
        return false;

    m_dataMemSize = calcDataMemSize();
    m_dspVarResolver.actualizeVarHash(); // wir setzen die hashtabelle neu
    return true;
}

void ZdspClient::setCmdListDef(const QString &cmdListDef)
{
    m_sCmdListDef = handleAndRemoveEntityId(cmdListDef);
}

void ZdspClient::setCmdForIrqListDef(const QString &cmdIntListDef)
{
    m_sIntCmdListDef = handleAndRemoveEntityId(cmdIntListDef);
}

QByteArray ZdspClient::getProtobufClientId() const
{
    return m_proxyConnectionId;
}

ulong ZdspClient::relocalizeUserMemSectionVars(ulong startAdress, ulong globalMemStart)
{
    ulong usermemsize = 0;
    ulong globalmemsize = 0;
    m_userMemSection.m_startAddress = startAdress;
    m_localVarDump = std::make_unique<QList<VarLocation>>();
    m_globalVarDump = std::make_unique<QList<VarLocation>>();

    for (int i = 0; i < m_userMemSection.getVarCount(); i++) {
        DspVarServerPtr dspVar = m_userMemSection.getDspVar(i);
        if (dspVar->segment == moduleLocalSegment) {
            dspVar->adr = startAdress + usermemsize; // we need the adress for reading back data
            m_localVarDump->append( { dspVar->Name,
                                      dspVar->offs,
                                      dspVar->adr } );
            usermemsize += dspVar->size;
        }
        else if (dspVar->segment == moduleGlobalSegment) {
            dspVar->adr = globalMemStart+globalmemsize;
            m_globalVarDump->append( { dspVar->Name,
                                       dspVar->offs,
                                       dspVar->adr } );
            globalmemsize += dspVar->size;
        }
    }
    return usermemsize;
}

bool ZdspClient::GenCmdLists(QString& errs, ulong userMemOffset, ulong globalstartadr)
{
    DspCmdCompiler compiler(&m_dspVarResolver, m_dspInterruptId);
    m_rawCyclicCommands = m_zdspSupportFactory->createDspCompilerSupport();
    m_rawInterruptCommands = m_zdspSupportFactory->createDspCompilerSupport();
    return
        compiler.compileCmds(m_sCmdListDef, m_DspCmdList,errs, userMemOffset, globalstartadr, m_rawCyclicCommands) &&
        compiler.compileCmds(m_sIntCmdListDef, m_DspIntCmdList, errs, userMemOffset, globalstartadr, m_rawInterruptCommands);
}

const QList<DspCmdWithParamsRaw> &ZdspClient::GetDspCmdList() const
{
    return m_DspCmdList;
}

const QList<DspCmdWithParamsRaw> &ZdspClient::GetDspIntCmdList() const
{
    return m_DspIntCmdList;
}

const QStringList &ZdspClient::getDspCmdListRaw() const
{
    return m_rawCyclicCommands->getRawDspCommands();
}

const QStringList &ZdspClient::getDspIntCmdListRaw() const
{
    return m_rawInterruptCommands->getRawDspCommands();
}

quint32 ZdspClient::getDspCmdListCompiledCrc() const
{
    PseudoCrcBuffer crcBuffer;
    crcBuffer.open(QIODevice::WriteOnly);
    QDataStream stream(&crcBuffer);
    stream.setByteOrder(QDataStream::LittleEndian);

    const QList<DspCmdWithParamsRaw> &cmdList = GetDspCmdList();
    for (const DspCmdWithParamsRaw &cmd : cmdList)
        stream << (quint32) cmd.w[0] << (quint32) cmd.w[1];
    crcBuffer.close();
    return crcBuffer.getCrc();
}

quint32 ZdspClient::getDspIntCmdCompiledCrc() const
{
    PseudoCrcBuffer crcBuffer;
    crcBuffer.open(QIODevice::WriteOnly);
    QDataStream stream(&crcBuffer);
    stream.setByteOrder(QDataStream::LittleEndian);

    const QList<DspCmdWithParamsRaw> &cmdList = GetDspIntCmdList();
    for (const DspCmdWithParamsRaw &cmd : cmdList)
        stream << (quint32) cmd.w[0] << (quint32) cmd.w[1];
    crcBuffer.close();
    return crcBuffer.getCrc();
}

int ZdspClient::getDspInterruptId() const
{
    return m_dspInterruptId;
}

int ZdspClient::getDataMemSize() const
{
    return m_dataMemSize;
}

VeinTcp::TcpPeer *ZdspClient::getVeinPeer() const
{
    return m_veinPeer;
}

int ZdspClient::getInstanceCount()
{
    return m_instanceCount;
}

int ZdspClient::calcDataMemSize()
{
    const int varCount = m_userMemSection.getVarCount();
    int dataMemSize = 0;
    for (int var=0; var<varCount; ++var)
        dataMemSize += m_userMemSection.getDspVar(var)->size;
    return dataMemSize;
}

QString ZdspClient::handleAndRemoveEntityId(const QString &scpiParam)
{
    const QString entityIdPrefixReceived = " #ENTID#"; // wherever the space comes from...
    if (!scpiParam.startsWith(entityIdPrefixReceived))
        return scpiParam;

    int firstSemicolonPos = scpiParam.indexOf(";", entityIdPrefixReceived.size());
    QString fullPrefix = scpiParam.left(firstSemicolonPos);
    QString entityIdStr = fullPrefix.right(fullPrefix.size() - entityIdPrefixReceived.size());
    int entityId = entityIdStr.toInt();
    setEntityId(entityId);

    QString scpiParamNew = " " + scpiParam.mid(firstSemicolonPos+1);
    return scpiParamNew;
}
