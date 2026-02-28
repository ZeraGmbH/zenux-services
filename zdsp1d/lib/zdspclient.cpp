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

bool ZdspClient::setRawActualValueList(const QString &varsSemicolonSeparated)
{
    m_dspVarArray.clear();
    int localOffset = 0;
    int globaloffset = 0;
    const QString varsSemicolonSeparatedEntityIdStripped = handleAndRemoveEntityId(varsSemicolonSeparated);
    const QStringList varEntries = varsSemicolonSeparatedEntityIdStripped.split(";", Qt::SkipEmptyParts);
    bool allOk = true;
    for(int i=0; i<varEntries.count(); i++) {
        TDspVar dspVar;
        if(dspVar.Init(varEntries[i])) {
            if (dspVar.segment == dspInternalSegment) {
                const TDspVar* dspVarDsp = m_dspVarResolver.getDspVar(dspVar.Name);
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
                m_dspVarArray.append(dspVar);
            }
            else if (dspVar.segment == moduleGlobalSegment) {
                dspVar.offs = globaloffset;
                // TODO already there / other clients????
                globaloffset += dspVar.size;
                m_dspVarArray.append(dspVar);
            }
        }
        else
            allOk = false;
    }
    if (!allOk)
        return false;

    m_userMemSection.m_varCount = m_dspVarArray.count();
    m_dataMemSize = calcDataMemSize(m_dspVarArray);

    if(m_userMemSection.m_varCount > 0) { // wir haben mindestens 1 variable
        // WTF!!!!!!!!!!!!!
        m_userMemSection.m_dspVars = m_dspVarArray.data();
    }
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

    for (int i = 0; i < m_userMemSection.m_varCount; i++) {
        if (m_userMemSection.m_dspVars[i].segment == moduleLocalSegment) {
            m_userMemSection.m_dspVars[i].adr = startAdress + usermemsize; // we need the adress for reading back data
            m_localVarDump->append( { m_userMemSection.m_dspVars[i].Name,
                                      m_userMemSection.m_dspVars[i].offs,
                                      m_userMemSection.m_dspVars[i].adr } );
            usermemsize += m_userMemSection.m_dspVars[i].size;
        }
        else if (m_userMemSection.m_dspVars[i].segment == moduleGlobalSegment) {
            m_userMemSection.m_dspVars[i].adr = globalMemStart+globalmemsize;
            m_globalVarDump->append( { m_userMemSection.m_dspVars[i].Name,
                                       m_userMemSection.m_dspVars[i].offs,
                                       m_userMemSection.m_dspVars[i].adr } );
            globalmemsize += m_userMemSection.m_dspVars[i].size;
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

int ZdspClient::calcDataMemSize(const QVector<TDspVar> &dspVarArray)
{
    const int varCount = dspVarArray.count();
    int dataMemSize = 0;
    for (int var=0; var<varCount; ++var)
        dataMemSize += dspVarArray[var].size;
    return dataMemSize;
}

QString ZdspClient::handleAndRemoveEntityId(const QString &scpiParam)
{
    const QString entityIdPrefixReceived = " #ENTID#"; // wherever the space comes from...
    if (!scpiParam.startsWith(entityIdPrefixReceived))
        return scpiParam;

    int firstSemicolonPos = scpiParam.indexOf(";", entityIdPrefixReceived.count());
    QString fullPrefix = scpiParam.left(firstSemicolonPos);
    QString entityIdStr = fullPrefix.right(fullPrefix.count() - entityIdPrefixReceived.count());
    int entityId = entityIdStr.toInt();
    setEntityId(entityId);

    QString scpiParamNew = " " + scpiParam.mid(firstSemicolonPos+1);
    return scpiParamNew;
}

QString ZdspClient::readActValues(const QString& variablesStringOnEmptyActOnly)
{
    QString variablesStringWithActual = variablesStringOnEmptyActOnly;
    if(variablesStringWithActual.isEmpty()) { // sonderfall liste leer -> alle messwerte lesen
        for(const TDspVar &dspVar : qAsConst(m_dspVarArray))
            variablesStringWithActual += QString("%1,%2;").arg(dspVar.Name).arg(dspVar.size);
    }
    DspVarDeviceNodeInOut dspInOut(m_zdspSupportFactory);
    return dspInOut.readDspVarList(variablesStringWithActual, &m_dspVarResolver);
}
