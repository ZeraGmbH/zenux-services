#include "zdspdumpfunctions.h"
#include "testdspcompilersupport.h"
#include <crc32isohdlc.h>
#include <QJsonArray>
#include <pseudocrcbuffer.h>

QJsonObject ZDspDumpFunctions::getStaticMemAllocation()
{
    QMap<QString /*clientHandleName*/, QMap<QString /* varName */, DspVarServerPtr>> varsSorted;
    const QHash<QString, DspVarServerPtr> staticVariables = DspStaticData::getVarHash();
    for (auto iter=staticVariables.cbegin(); iter!=staticVariables.cend(); ++iter) {
        const DspVarServerPtr dspVar = iter.value();
        varsSorted[dspVar->m_clientVarGroupName][dspVar->Name] = dspVar;
    }
    QJsonObject json;
    for (auto iterClient=varsSorted.cbegin(); iterClient!=varsSorted.cend(); ++iterClient) {
        QJsonObject jsonVars;
        for (auto iterVars=iterClient->cbegin(); iterVars!=iterClient->cend(); ++iterVars) {
            QJsonObject jsonVar;
            QString hexvalue = DspVarInServer::toHex(iterVars.value()->m_absoluteAddress);
            jsonVar.insert("Addr", hexvalue);
            hexvalue = DspVarInServer::toHex(iterVars.value()->m_offsetToModuleBase);
            jsonVar.insert("Offset", hexvalue);
            jsonVar.insert("Size", int(iterVars.value()->size));
            hexvalue = DspVarInServer::toHex(iterVars.value()->size);
            jsonVar.insert("SizeHex", hexvalue);

            const QString varName = iterVars.key();
            jsonVars.insert(varName, jsonVar);
        }
        const QString clientHandleName = iterClient.key();
        json.insert(clientHandleName, jsonVars);
    }
    return json;
}

QJsonObject ZDspDumpFunctions::getFullDump(const ZDspServer *server)
{
    QJsonObject json;
    json.insert("ProgMemCyclicAvailable", server->getProgMemCyclicAvailable());
    json.insert("ProgMemCyclicFree", server->getProgMemCyclicAvailable()-server->getProgMemCyclicOccupied());
    json.insert("ProgMemCyclicUsed", server->getProgMemCyclicOccupied());
    json.insert("ProgMemInterruptAvailable", server->getProgMemInterruptAvailable());
    json.insert("ProgMemInterruptFree", server->getProgMemInterruptAvailable() - server->getProgMemInterruptOccupied());
    json.insert("ProgMemInterruptUsed", server->getProgMemInterruptOccupied());
    json.insert("UserMemAvailable", server->getVarMemLocalAvailable());
    json.insert("UserMemFree", server->getVarMemLocalAvailable() - server->getVarMemOccupied(moduleLocalSegment));
    json.insert("UserMemUsed", server->getVarMemOccupied(moduleLocalSegment));
    json.insert("UserMemAlignedAvailable", server->getVarMemAlignedAvailable());
    json.insert("UserMemAlignedFree", server->getVarMemAlignedAvailable() - server->getVarMemOccupied(moduleAlignedMemorySegment));
    json.insert("UserMemAlignedUsed", server->getVarMemOccupied(moduleAlignedMemorySegment));
    json.insert("ZdspMemDump", getMemoryDump(server));
    return json;
}

int ZDspDumpFunctions::dumpCmdListCompiledList(QJsonArray &cyclicCmdsOut,
                                               const QStringList &cyclicCmdListIn,
                                               const QList<DspCmdWithParamsCompiled> &compiledListIn)
{
    int cyclCmdListCount = 0;
    for (int i=0; i<int(cyclicCmdListIn.size()); ++i) {
        const QString &cmd = cyclicCmdListIn[i];
        if (!cmd.startsWith("--")) {
            cyclicCmdsOut.append(cyclicCmdListIn[i]+ " -> " +
                                 crcToHex(compiledListIn[cyclCmdListCount].w[0]) + " " +
                                 crcToHex(compiledListIn[cyclCmdListCount].w[1]));
            cyclCmdListCount++;
        }
        else
            cyclicCmdsOut.append(cyclicCmdListIn[i]);
    }
    return cyclCmdListCount;
}

QJsonObject ZDspDumpFunctions::getMemoryDump(const ZDspServer *server)
{
    const QList<ZdspClient*> &clientList = server->getClients();
    QJsonObject json;
    for (const ZdspClient* client : clientList) {
        int entityId = client->getEntityId();
        if (entityId >= 0) {
            QJsonObject entityData;

            const QStringList &cyclicCmdList = client->getCurrCyclicCommandsCompilerSupport()->getRawDspCommands(AbstractDspCompilerSupport::CYCLIC);
            const QList<DspCmdWithParamsCompiled> &cyclicCompiledList = client->GetDspCmdList();
            QJsonArray cyclicCmds;
            int cyclCmdListCount = dumpCmdListCompiledList(cyclicCmds, cyclicCmdList, cyclicCompiledList);
            if (cyclCmdListCount != cyclicCompiledList.size())
                qFatal("Entity Id: %i: Cyclic Cmd(%i)/Compiled(%i) mismatch!",
                       entityId, cyclCmdListCount, int(cyclicCompiledList.size()));
            entityData.insert("DspCmdsRawCyclic", cyclicCmds);
            entityData.insert("DspCmdsCompiledCrcCyclic", crcToHex(getDspCmdListCompiledCrc(client->GetDspCmdList())));

            const QStringList &intCmdList = client->getCurrInterruptCommandsCompilerSupport()->getRawDspCommands(AbstractDspCompilerSupport::INTERRUPT);
            const QList<DspCmdWithParamsCompiled> &intCompiledList = client->GetDspIntCmdList();
            QJsonArray intCmds;
            int intCmdListCount = dumpCmdListCompiledList(intCmds, intCmdList, intCompiledList);
            if (intCmdListCount != intCompiledList.size())
                qFatal("Entity Id: %i: Interrupt Cmd(%i)/Compiled(%i) mismatch!",
                       entityId, intCmdListCount, int(intCompiledList.size()));
            entityData.insert("DspCmdsRawInterrupt", intCmds);
            entityData.insert("DspCmdsCompiledCrcInterrupt", crcToHex(getDspCmdListCompiledCrc(client->GetDspIntCmdList())));

            const DspMemorySectionInternal &memSection = client->getUserMemSection();
            entityData.insert("DspVarsClientBaseAddress", DspVarInServer::toHex(memSection.m_startAddress));

            entityData.insert("DspVarsLocal", dumpVariables(getVariableDump(memSection, moduleLocalSegment)));
            entityData.insert("DspVarsAligned", dumpVariables(getVariableDump(memSection, moduleAlignedMemorySegment)));
            entityData.insert("DspVarsGlobal", dumpVariables(getVariableDump(memSection, moduleGlobalSegment)));

            json.insert(QString("%1").arg(entityId), entityData);
        }
    }
    QString dummy;
    QByteArray rawCyclicCmdMem, rawInterruptCmdMem;
    server->compileCmdListsForAllClientsToBinaryStream(dummy, rawCyclicCmdMem, rawInterruptCmdMem);

    quint32 crcTotalCyclic = Crc32IsoHdlc::calcCrc32(rawCyclicCmdMem.constData(), rawCyclicCmdMem.size());
    QString crcTotalCyclicStr = crcToHex(crcTotalCyclic);
    json.insert("TotalDspCmdsCompiledCrcCyclic", crcTotalCyclicStr);

    quint32 crcTotalInterrupt = Crc32IsoHdlc::calcCrc32(rawInterruptCmdMem.constData(), rawInterruptCmdMem.size());
    QString crcTotalInterruptStr = crcToHex(crcTotalInterrupt);
    json.insert("TotalDspCmdsCompiledCrcInterrupt", crcTotalInterruptStr);

    const QStringList &totalCmdStrListCyclic = TestDspCompilerSupport::getRawDspCommandsAllVerbose(AbstractDspCompilerSupport::CYCLIC);
    QJsonArray totalCmdListCyclic = QJsonArray::fromStringList(totalCmdStrListCyclic);
    json.insert("TotalCmdListCyclic", totalCmdListCyclic);
    const QStringList &totalCmdStrListInterrupt = TestDspCompilerSupport::getRawDspCommandsAllVerbose(AbstractDspCompilerSupport::INTERRUPT);
    QJsonArray totalCmdListInterrupt = QJsonArray::fromStringList(totalCmdStrListInterrupt);
    json.insert("TotalCmdListInterrupt", totalCmdListInterrupt);

    const int interruptCountCyclic = TestDspCompilerSupport::getRawDspIntTriggerCount(AbstractDspCompilerSupport::CYCLIC);
    json.insert("TotalCmdListCyclicMaxInterruptCount", interruptCountCyclic);
    const int interruptCountInterrupt = TestDspCompilerSupport::getRawDspIntTriggerCount(AbstractDspCompilerSupport::INTERRUPT);
    json.insert("TotalCmdListInterruptMaxInterruptCount", interruptCountInterrupt);
    const int maxInterruptsExpected = interruptCountCyclic+interruptCountInterrupt;
    if (maxInterruptsExpected > DSP_MAX_PENDING_INTERRUPT_COUNT)
        qCritical("Max possible interrupts exceeded: Allowed: %i / Wanted: %i",
                  DSP_MAX_PENDING_INTERRUPT_COUNT, maxInterruptsExpected);

    // Sanity check cyclic
    int rawCmdCountCyclic = TestDspCompilerSupport::getRawDspCommandsCount(AbstractDspCompilerSupport::CYCLIC);
    int binaryCmdCountCyclic = rawCyclicCmdMem.size() / 8;
    if (rawCmdCountCyclic != binaryCmdCountCyclic - 2) // -2: comiler support does not count INVALID() / DSPINTPOST
        qCritical("Cyclic cross check failed: Support: %i / binary %i", rawCmdCountCyclic, binaryCmdCountCyclic);
    int announcedCyclic = server->getProgMemCyclicOccupied();
    if (announcedCyclic != binaryCmdCountCyclic)
        qCritical("Cyclic cross check failed: Announced: %i / binary %i", announcedCyclic, binaryCmdCountCyclic);

    // Sanity check interrupt
    int rawCmdCountInterrupt = TestDspCompilerSupport::getRawDspCommandsCount(AbstractDspCompilerSupport::INTERRUPT);
    int binaryCmdCountInterrupt = rawInterruptCmdMem.size() / 8;
    if (rawCmdCountInterrupt != binaryCmdCountInterrupt-1) // -1: comiler support does not count INVALID()
        qCritical("Interrupt cross check failed: Support: %i / binary %i", rawCmdCountInterrupt, binaryCmdCountInterrupt);
    int announcedInterrupt = server->getProgMemInterruptOccupied();
    if (announcedInterrupt != binaryCmdCountInterrupt)
        qCritical("Interrupt cross check failed: Announced: %i / binary %i", announcedInterrupt, binaryCmdCountInterrupt);

    return json;
}

quint32 ZDspDumpFunctions::getDspCmdListCompiledCrc(const QList<DspCmdWithParamsCompiled> &cmdList)
{
    PseudoCrcBuffer crcBuffer;
    crcBuffer.open(QIODevice::WriteOnly);
    QDataStream stream(&crcBuffer);
    stream.setByteOrder(QDataStream::LittleEndian);

    for (const DspCmdWithParamsCompiled &cmd : cmdList)
        stream << (quint32) cmd.w[0] << (quint32) cmd.w[1];
    crcBuffer.close();
    return crcBuffer.getCrc();
}

QList<ZDspDumpFunctions::VarLocation> ZDspDumpFunctions::getVariableDump(const DspMemorySectionInternal &clientUserMemSection,
                                                                         DspSegmentType segment)
{
    QList<VarLocation> alignedVarDump;
    for (int i = 0; i < clientUserMemSection.getVarCount(); i++) {
        DspVarServerPtr dspVar = clientUserMemSection.getDspVar(i);
        if (dspVar->segment == segment)
            alignedVarDump.append( { dspVar->Name,
                                     dspVar->m_offsetToModuleBase,
                                     dspVar->m_absoluteAddress } );
    }
    return alignedVarDump;
}

QString ZDspDumpFunctions::crcToHex(quint32 val)
{
    QString hexVal = QString("00000000%1").arg(val, 0, 16).toUpper().right(8);
    return QString("0x") + hexVal;
}

QJsonObject ZDspDumpFunctions::dumpVariables(const QList<VarLocation> &vars)
{
    QJsonObject dumpTarget;
    for (const VarLocation &var : vars) {
        QString key = QString("%1 / %2").arg(
            DspVarInServer::toHex(var.m_localVariableAddress), DspVarInServer::toHex(var.m_absoluteVariableAddress));
        dumpTarget.insert(key, var.m_variableName);
    }
    return dumpTarget;
}
