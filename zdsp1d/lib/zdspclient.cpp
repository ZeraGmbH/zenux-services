#include "zdspclient.h"
#include "dspcmdcompiler.h"
#include <netmessages.pb.h>

int ZdspClient::m_instanceCount = 0;
QHash<QString, DspVarServerPtr> ZdspClient::m_globalVariables;
ulong ZdspClient::m_globalVarSize = 0;

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
    if (m_instanceCount == 0)
        resetGlobalVarList();
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
    const QStringList varEntries = varsSemicolonSeparated.split(";", Qt::SkipEmptyParts);
    for(int i=0; i<varEntries.count(); i++) {
        DspVarInServer dspVar;
        if (!dspVar.setupFromCommaSeparatedString(varEntries[i]))
            return false;

        if (dspVar.segment == moduleGlobalSegment) {
            const QString &varName = dspVar.Name;
            auto iter = m_globalVariables.constFind(varName);
            if (iter != m_globalVariables.constEnd() && dspVar.size != iter.value()->size) {
                qCritical("Size change on global DSP Variable %s detected. Found %i / Wanted %i",
                          qPrintable(dspVar.Name), iter.value()->size, dspVar.size);
                return false;
            }
        }
        else if (dspVar.segment == dspInternalSegment) {
            const DspVarServerPtr dspVarDsp = m_dspVarResolver.getDspVar(dspVar.Name);
            if (dspVarDsp == nullptr) {
                qCritical("Internal DSP Variable %s not found",
                          qPrintable(dspVar.Name));
                return false;
            }
            else if (dspVarDsp->size != dspVar.size) {
                qCritical("Internal DSP Variable %s has wrong size: wanted: %i / expected %i",
                          qPrintable(dspVar.Name), dspVar.size, dspVarDsp->size);
                return false;
            }
            else if (dspVarDsp->type != dspVar.type) {
                qCritical("Internal DSP Variable %s has wrong type: wanted: %i / expected %i",
                          qPrintable(dspVar.Name), dspVar.type, dspVarDsp->type);
                return false;
            }
        }
        m_userMemSection.appendDspVar(dspVar);
    }

    m_dspVarResolver.actualizeVarHash();
    return true;
}

void ZdspClient::resetGlobalVarList()
{
    m_globalVariables.clear();
    m_globalVarSize = 0;
}

void ZdspClient::setCmdListDef(const QString &cmdListDef)
{
    m_sCmdListDef = cmdListDef;
}

void ZdspClient::setCmdForIrqListDef(const QString &cmdIntListDef)
{
    m_sIntCmdListDef = cmdIntListDef;
}

void ZdspClient::sendInterruptNotification(ulong parameter, XiQNetWrapper &protobufWrapper) const
{
    ProtobufMessage::NetMessage protobufIntMessage;
    ProtobufMessage::NetMessage::NetReply *intMessage = protobufIntMessage.mutable_reply();

    const QString dspIntStr = QString("DSPINT:%1").arg(parameter & 0xFFFF);
    intMessage->set_body(dspIntStr.toStdString());
    intMessage->set_rtype(ProtobufMessage::NetMessage_NetReply_ReplyType_ACK);

    const QByteArray proxyConnectionId = getProtobufClientId();
    protobufIntMessage.set_clientid(proxyConnectionId.data(), proxyConnectionId.size());
    protobufIntMessage.set_messagenr(0); // interrupt

    getVeinPeer()->sendMessage(protobufWrapper.protobufToByteArray(protobufIntMessage));
}

const QByteArray &ZdspClient::getProtobufClientId() const
{
    return m_proxyConnectionId;
}

void ZdspClient::calcVarAdressesAndSizes(ulong &currLocalAbsAdr,
                                         ulong &currAlignedAbsAdr,
                                         ulong &currGlobalAbsAdr)
{
    m_userMemSection.m_startAddress = currLocalAbsAdr;
    ulong clientStartAbsAdr = currLocalAbsAdr;

    for (int i = 0; i < m_userMemSection.getVarCount(); i++) {
        DspVarServerPtr dspVar = m_userMemSection.getDspVar(i);
        const int varSize = dspVar->size;
        if (dspVar->segment == moduleLocalSegment) {
            dspVar->m_absoluteAddress = currLocalAbsAdr;
            dspVar->m_offsetToModuleBase = currLocalAbsAdr - clientStartAbsAdr;
            currLocalAbsAdr += varSize;
        }
        else if (dspVar->segment == moduleAlignedMemorySegment) {
            dspVar->m_absoluteAddress = currAlignedAbsAdr;
            dspVar->m_offsetToModuleBase = currAlignedAbsAdr - clientStartAbsAdr;
            currAlignedAbsAdr += varSize;
        }
        else if(dspVar->segment == moduleGlobalSegment) {
            const QString &varName = dspVar->Name;
            auto iter = m_globalVariables.constFind(varName);
            if (iter == m_globalVariables.constEnd()) {
                dspVar->m_absoluteAddress = currGlobalAbsAdr;
                dspVar->m_offsetToModuleBase = currGlobalAbsAdr - clientStartAbsAdr;
                // global mem pointer counts down into local segment
                currGlobalAbsAdr -= varSize;
                m_globalVarSize += varSize;
                m_globalVariables[varName] = dspVar;
            }
            else {
                ulong absAddress = iter.value()->m_absoluteAddress;
                dspVar->m_absoluteAddress = absAddress;
                dspVar->m_offsetToModuleBase = absAddress - clientStartAbsAdr;
            }
        }
    }
}

bool ZdspClient::compileCmdLists(QString& errs)
{
    DspCmdCompiler compiler(&m_dspVarResolver, m_dspInterruptId);
    m_cyclicCommandsCompilerSupport = m_zdspSupportFactory->createDspCompilerSupport();
    m_interruptCommandsCompilerSupport = m_zdspSupportFactory->createDspCompilerSupport();

    if (!m_sCmdListDef.isEmpty()) {
        getCurrCyclicCommandsCompilerSupport()->startClientArea(getEntityId(), "Cyclic sequence", AbstractDspCompilerSupport::CYCLIC);
        if(!compiler.compileCmds(m_sCmdListDef, m_DspCmdList, errs, m_cyclicCommandsCompilerSupport))
            return false;
    }
    if (!m_sIntCmdListDef.isEmpty()) {
        getCurrCyclicCommandsCompilerSupport()->startClientArea(getEntityId(), "Interrupt sequence", AbstractDspCompilerSupport::INTERRUPT);
        if(!compiler.compileCmds(m_sIntCmdListDef, m_DspIntCmdList, errs, m_interruptCommandsCompilerSupport))
            return false;
    }
    return true;
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

int ZdspClient::getMemSize(DspSegmentType segment) const
{
    const int varCount = m_userMemSection.getVarCount();
    int memSize = 0;
    for (int var=0; var<varCount; ++var) {
        DspVarServerPtr dspVar = m_userMemSection.getDspVar(var);
        if (dspVar->segment == segment)
            memSize += m_userMemSection.getDspVar(var)->size;
    }
    return memSize;
}

int ZdspClient::getGlobalMemSizeTotal()
{
    return m_globalVarSize;
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
