#include "zdspdumpfunctions.h"
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
            QString hexvalue = DspVarInServer::toHex(iterVars.value()->adr);
            jsonVar.insert("Addr", hexvalue);
            hexvalue = DspVarInServer::toHex(iterVars.value()->offs);
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

            QJsonArray cyclicCmds = QJsonArray::fromStringList(client->getDspCmdListRaw());
            entityData.insert("DspCmdsRawCyclic", cyclicCmds);
            entityData.insert("DspCmdsCompiledCrcCyclic", crcToHex(getDspCmdListCompiledCrc(client->GetDspCmdList())));

            QJsonArray interruptCmds = QJsonArray::fromStringList(client->getDspIntCmdListRaw());
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

            QJsonObject globalVariables;
            const QList<VarLocation> globalList = getGlobalVariableDump(client->getUserMemSection());
            for (const VarLocation &entry : globalList) {
                QString key = QString("%1 / %2").arg(
                    DspVarInServer::toHex(entry.m_localVariableAddress), DspVarInServer::toHex(entry.m_absoluteVariableAddress));
                globalVariables.insert(key, entry.m_variableName);
            }
            entityData.insert("DspVarsGlobal", globalVariables);

            json.insert(QString("%1").arg(entityId), entityData);
        }
    }
    QString dummy;
    QByteArray rawCyclicCmdMem, rawInterruptCmdMem;
    server->compileCmdListsForAllClientsToRawStream(dummy, rawCyclicCmdMem, rawInterruptCmdMem);

    quint32 crcTotalCyclic = Crc32IsoHdlc::calcCrc32(rawCyclicCmdMem.constData(), rawCyclicCmdMem.size());
    QString crcTotalCyclicStr = crcToHex(crcTotalCyclic);
    json.insert("TotalDspCmdsCompiledCrcCyclic", crcTotalCyclicStr);

    quint32 crcTotalInterrupt = Crc32IsoHdlc::calcCrc32(rawInterruptCmdMem.constData(), rawInterruptCmdMem.size());
    QString crcTotalInterruptStr = crcToHex(crcTotalInterrupt);
    json.insert("TotalDspCmdsCompiledCrcInterrupt", crcTotalInterruptStr);

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

QList<ZDspDumpFunctions::VarLocation> ZDspDumpFunctions::getGlobalVariableDump(const DspMemorySectionInternal &clientUserMemSection)
{
    QList<VarLocation> globalVarDump;
    for (int i = 0; i < clientUserMemSection.getVarCount(); i++) {
        DspVarServerPtr dspVar = clientUserMemSection.getDspVar(i);
        if (dspVar->segment == moduleGlobalSegment)
            globalVarDump.append( { dspVar->Name,
                                  dspVar->offs,
                                  dspVar->adr } );
    }
    return globalVarDump;
}

QList<ZDspDumpFunctions::VarLocation> ZDspDumpFunctions::getLocalVariableDump(const DspMemorySectionInternal &clientUserMemSection)
{
    QList<VarLocation> localVarDump;
    for (int i = 0; i < clientUserMemSection.getVarCount(); i++) {
        DspVarServerPtr dspVar = clientUserMemSection.getDspVar(i);
        if (dspVar->segment == moduleLocalSegment)
            localVarDump.append( { dspVar->Name,
                                 dspVar->offs,
                                 dspVar->adr } );
    }
    return localVarDump;
}

QString ZDspDumpFunctions::crcToHex(quint32 val)
{
    QString hexVal = QString("00000000%1").arg(val, 0, 16).toUpper().right(8);
    return QString("0x") + hexVal;
}
