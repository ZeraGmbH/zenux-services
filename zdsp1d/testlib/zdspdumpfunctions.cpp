#include "zdspdumpfunctions.h"

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
