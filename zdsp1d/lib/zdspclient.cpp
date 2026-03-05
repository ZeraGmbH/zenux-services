#include "zdspclient.h"
#include "dspcmdcompiler.h"
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
    m_cyclicCommandsCompilerSupport(m_zdspSupportFactory->createDspCompilerSupport()),
    m_interruptCommandsCompilerSupport(m_zdspSupportFactory->createDspCompilerSupport())
{
    m_instanceCount++;
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

bool ZdspClient::setVarList(const QString &varsSemicolonSeparated)
{
    m_userMemSection.clear();
    int localOffset = 0;
    int globaloffset = 0;
    const QStringList varEntries = varsSemicolonSeparated.split(";", Qt::SkipEmptyParts);
    bool allOk = true;
    for(int i=0; i<varEntries.count(); i++) {
        DspVarInServer dspVar;
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
    m_sCmdListDef = cmdListDef;
}

void ZdspClient::setCmdForIrqListDef(const QString &cmdIntListDef)
{
    m_sIntCmdListDef = cmdIntListDef;
}

const QByteArray &ZdspClient::getProtobufClientId() const
{
    return m_proxyConnectionId;
}

ulong ZdspClient::relocalizeUserMemSectionVars(ulong startAdress, ulong globalMemStart)
{
    ulong usermemsize = 0;
    ulong globalmemsize = 0;
    m_userMemSection.m_startAddress = startAdress;

    for (int i = 0; i < m_userMemSection.getVarCount(); i++) {
        DspVarServerPtr dspVar = m_userMemSection.getDspVar(i);
        if (dspVar->segment == moduleLocalSegment) {
            dspVar->adr = startAdress + usermemsize; // we need the adress for reading back data
            usermemsize += dspVar->size;
        }
        else if (dspVar->segment == moduleGlobalSegment) {
            dspVar->adr = globalMemStart+globalmemsize;
            globalmemsize += dspVar->size;
        }
    }
    return usermemsize;
}

bool ZdspClient::GenCmdLists(QString& errs, ulong userMemOffset, ulong globalstartadr)
{
    DspCmdCompiler compiler(&m_dspVarResolver, m_dspInterruptId);
    m_cyclicCommandsCompilerSupport = m_zdspSupportFactory->createDspCompilerSupport();
    m_interruptCommandsCompilerSupport = m_zdspSupportFactory->createDspCompilerSupport();

    bool ok = true;
    if (!m_sCmdListDef.isEmpty()) {
        getCurrCyclicCommandsCompilerSupport()->startClientArea(getEntityId(), "Cyclic sequence", AbstractDspCompilerSupport::CYCLIC);
        ok = ok && compiler.compileCmds(m_sCmdListDef, m_DspCmdList,errs, userMemOffset, globalstartadr, m_cyclicCommandsCompilerSupport);
    }
    if (!m_sIntCmdListDef.isEmpty()) {
        getCurrCyclicCommandsCompilerSupport()->startClientArea(getEntityId(), "Interrupt sequence", AbstractDspCompilerSupport::INTERRUPT);
        ok = ok && compiler.compileCmds(m_sIntCmdListDef, m_DspIntCmdList, errs, userMemOffset, globalstartadr, m_interruptCommandsCompilerSupport);
    }
    return ok;
}

const QList<DspCmdWithParamsCompiled> &ZdspClient::GetDspCmdList() const
{
    return m_DspCmdList;
}

const QList<DspCmdWithParamsCompiled> &ZdspClient::GetDspIntCmdList() const
{
    return m_DspIntCmdList;
}

const DspMemorySectionInternal &ZdspClient::getUserMemSection() const
{
    return m_userMemSection;
}

AbstractDspCompilerSupportPtr ZdspClient::getCurrCyclicCommandsCompilerSupport() const
{
    return m_cyclicCommandsCompilerSupport;
}

AbstractDspCompilerSupportPtr ZdspClient::getCurrInterruptCommandsCompilerSupport() const
{
    return m_interruptCommandsCompilerSupport;
}

int ZdspClient::getDspInterruptId() const
{
    return m_dspInterruptId;
}

int ZdspClient::getDataMemSize() const
{
    return m_dataMemSize;
}

bool ZdspClient::hasCyclicCmds() const
{
    return !m_sCmdListDef.isEmpty();
}

bool ZdspClient::hasInterruptCmds() const
{
    return !m_sIntCmdListDef.isEmpty();
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
