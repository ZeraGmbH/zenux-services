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

QJsonObject ZDspDumpFunctions::getMemoryDump(const ZDspServer *server)
{
    const QList<ZdspClient*> &clientList = server->getClients();
    QJsonObject json;
    for (const ZdspClient* client : clientList) {
        int entityId = client->getEntityId();
        if (entityId >= 0) {
            QJsonObject entityData;

            QJsonArray cyclicCmds = QJsonArray::fromStringList(client->getCurrCyclicCommandsCompilerSupport()->getRawDspCommands(AbstractDspCompilerSupport::CYCLIC));
            entityData.insert("DspCmdsRawCyclic", cyclicCmds);
            entityData.insert("DspCmdsCompiledCrcCyclic", crcToHex(getDspCmdListCompiledCrc(client->GetDspCmdList())));

            QJsonArray interruptCmds = QJsonArray::fromStringList(client->getCurrInterruptCommandsCompilerSupport()->getRawDspCommands(AbstractDspCompilerSupport::INTERRUPT));
            entityData.insert("DspCmdsRawInterrupt", interruptCmds);
            entityData.insert("DspCmdsCompiledCrcInterrupt", crcToHex(getDspCmdListCompiledCrc(client->GetDspIntCmdList())));

            QJsonObject localVariables;
            const QList<VarLocation> localList = getLocalVariableDump(client->getUserMemSection());
            for (const VarLocation &entry : localList) {
                QString key = QString("%1 / %2").arg(
                    DspVarInServer::toHex(entry.m_localVariableAddress), DspVarInServer::toHex(entry.m_absoluteVariableAddress));
                localVariables.insert(key, entry.m_variableName);
            }
            entityData.insert("DspVarsLocal", localVariables);

            QJsonObject alignedVariables;
            const QList<VarLocation> alignedList = getAlignedVariableDump(client->getUserMemSection());
            for (const VarLocation &entry : alignedList) {
                QString key = QString("%1 / %2").arg(
                    DspVarInServer::toHex(entry.m_localVariableAddress), DspVarInServer::toHex(entry.m_absoluteVariableAddress));
                alignedVariables.insert(key, entry.m_variableName);
            }
            entityData.insert("DspVarsAligned", alignedVariables);

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

    QJsonArray totalCmdListCyclic = QJsonArray::fromStringList(TestDspCompilerSupport::getRawDspCommandsAllVerbose(AbstractDspCompilerSupport::CYCLIC));
    json.insert("TotalCmdListCyclic", totalCmdListCyclic);
    QJsonArray totalCmdListInterrupt = QJsonArray::fromStringList(TestDspCompilerSupport::getRawDspCommandsAllVerbose(AbstractDspCompilerSupport::INTERRUPT));
    json.insert("TotalCmdListInterrupt", totalCmdListInterrupt);

    // Sanity check cyclic
    int rawCmdCountCyclic = TestDspCompilerSupport::getRawDspCommandsCount(AbstractDspCompilerSupport::CYCLIC);
    int binaryCmdCountCyclic = rawCyclicCmdMem.count() / 8;
    if (rawCmdCountCyclic != binaryCmdCountCyclic-1) // -1: comiler support does not count INVALID()
        qCritical("Cyclic cross check failed: Support: %i / binary %i", rawCmdCountCyclic, binaryCmdCountCyclic);
    int announcedCyclic = server->getProgMemCyclicOccupied();
    if (announcedCyclic != binaryCmdCountCyclic)
        qCritical("Cyclic cross check failed: Announced: %i / binary %i", announcedCyclic, binaryCmdCountCyclic);

    // Sanity check interrupt
    int rawCmdCountInterrupt = TestDspCompilerSupport::getRawDspCommandsCount(AbstractDspCompilerSupport::INTERRUPT);
    int binaryCmdCountInterrupt = rawInterruptCmdMem.count() / 8;
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

QList<ZDspDumpFunctions::VarLocation> ZDspDumpFunctions::getAlignedVariableDump(const DspMemorySectionInternal &clientUserMemSection)
{
    QList<VarLocation> alignedVarDump;
    for (int i = 0; i < clientUserMemSection.getVarCount(); i++) {
        DspVarServerPtr dspVar = clientUserMemSection.getDspVar(i);
        if (dspVar->segment == moduleAlignedMemorySegment)
            alignedVarDump.append( { dspVar->Name,
                                     dspVar->m_offsetToModuleBase,
                                     dspVar->m_absoluteAddress } );
    }
    return alignedVarDump;
}

QList<ZDspDumpFunctions::VarLocation> ZDspDumpFunctions::getLocalVariableDump(const DspMemorySectionInternal &clientUserMemSection)
{
    QList<VarLocation> localVarDump;
    for (int i = 0; i < clientUserMemSection.getVarCount(); i++) {
        DspVarServerPtr dspVar = clientUserMemSection.getDspVar(i);
        if (dspVar->segment == moduleLocalSegment)
            localVarDump.append( { dspVar->Name,
                                   dspVar->m_offsetToModuleBase,
                                   dspVar->m_absoluteAddress } );
    }
    return localVarDump;
}

QString ZDspDumpFunctions::crcToHex(quint32 val)
{
    QString hexVal = QString("00000000%1").arg(val, 0, 16).toUpper().right(8);
    return QString("0x") + hexVal;
}
