#include "clampinterface.h"
#include "clamp.h"
#include "senseinterfacecommon.h"
#include "i2csettings.h"
#include "zscpi_response_definitions.h"

cClampInterface::cClampInterface(PCBServer *server,
                                 cSenseSettingsPtr senseSettings,
                                 SenseInterfaceCommon *senseInterface,
                                 I2cSettingsPtr i2cSettings,
                                 AbstractEepromI2cFactoryPtr adjMemFactory,
                                 AbstractFactoryI2cCtrlPtr ctrlFactory) :
    ScpiConnection(server->getSCPIInterface()),
    m_pMyServer(server),
    m_senseSettings(senseSettings),
    m_pSenseInterface(senseInterface),
    m_i2cSettings(i2cSettings),
    m_adjMemFactory(adjMemFactory),
    m_ctrlFactory(ctrlFactory)
{
    m_nClampStatus = 0;
}

void cClampInterface::initSCPIConnection(const QString &leadingNodes)
{
    const QString adjLeadNodes = appendTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1SYSTEM:CLAMP:CHANNEL").arg(adjLeadNodes),"CATALOG",SCPI::isQuery, m_scpiInterface, ClampSystem::cmdClampChannelCat, &m_notifierClampChannelList);
    addDelegate(QString("%1SYSTEM:CLAMP").arg(adjLeadNodes),"WRITE",SCPI::isCmd, m_scpiInterface, ClampSystem::cmdClampWrite);
    addDelegate(QString("%1SYSTEM:ADJUSTMENT:CLAMP").arg(adjLeadNodes),"XML",SCPI::isQuery | SCPI::isCmdwP, m_scpiInterface, ClampSystem::cmdClampImportExport);
}

cClamp *cClampInterface::addClamp(const SenseSystem::cChannelSettings *chSettings, EepromI2cDeviceInterfacePtr adjMemory)
{
    m_nClampStatus |= (1<<chSettings->m_nPluggedBit);

    // assumption - hope we find better
    int phaseCount = m_senseSettings->getChannelSettings().size()/2;
    int ctlChannelSecondary = chSettings->m_nCtrlChannel - phaseCount;

    cClamp* clamp = new cClamp(m_pMyServer,
                               m_pSenseInterface,
                               chSettings,
                               std::move(adjMemory),
                               ctlChannelSecondary);
    m_clampHash[chSettings->m_nameMx] = clamp;
    qInfo("Add clamp ranges for \"%s\"", qPrintable(chSettings->m_sAlias1));
    const QString channelMNameSecondary = clamp->getChannelNameSecondary();
    if(!channelMNameSecondary.isEmpty()) {
        m_clampSecondarySet.insert(channelMNameSecondary);
        qInfo("Add secondary clamp ranges for \"%s\"", qPrintable(m_senseSettings->findChannelSettingByMxName(channelMNameSecondary)->m_sAlias1));
    }
    generateAndNotifyClampChannelList();
    return clamp;
}

QString cClampInterface::exportXMLString(int indent)
{
    QString xmlTotal;
    for(auto clamp : qAsConst(m_clampHash)) {
        QString xmlClamp = clamp->exportXMLString(indent);
        xmlTotal.append(xmlClamp);
    }
    return xmlTotal;
}

cClamp *cClampInterface::tryAddClamp(const SenseSystem::cChannelSettings *chSettings)
{
    EepromI2cDeviceInterfacePtr adjMemory = m_adjMemFactory->createEepromOnMux(
        {m_i2cSettings->getDeviceNode(), m_i2cSettings->getI2CAdress(i2cSettings::clampFlashI2cAddress)},
        AbstractEepromI2cDevice::capacity24LC256,
        {m_i2cSettings->getDeviceNode(), m_i2cSettings->getI2CAdress(i2cSettings::muxerI2cAddress)},
        chSettings->m_nMuxChannelNo);

    if(adjMemory->isMemoryPlugged()) // ignore other than flash
        return addClamp(chSettings, std::move(adjMemory));

    qInfo("Not a clamp on %s", qPrintable(chSettings->m_sAlias1));
    return nullptr;
}

void cClampInterface::handleClampDisconnected(QString channelName, const SenseSystem::cChannelSettings *chSettings, quint16 bmask)
{
    int ctrlChannel = chSettings->m_nCtrlChannel;
    if (m_clampHash.contains(channelName)) {
        m_nClampStatus &= (~bmask);
        cClamp* clamp = m_clampHash.take(channelName);
        qInfo("Remove clamp channel \"%s\"/%i", qPrintable(channelName), ctrlChannel);
        QString channelNameSecondary = clamp->getChannelNameSecondary();
        if(!channelNameSecondary.isEmpty())
            m_clampSecondarySet.remove(channelNameSecondary);
        generateAndNotifyClampChannelList();
        delete clamp;
    }
    else
        qWarning("Clamp \"%s\"/%i to remove not found!", qPrintable(channelName), ctrlChannel);
}

void cClampInterface::actualizeClampStatus(quint16 devConnectedMask)
{
    quint16 clChange = devConnectedMask ^ m_nClampStatus; // now we know which clamps changed
    const auto channelsSettings = m_senseSettings->getChannelSettings();
    for(const auto channelSettings : channelsSettings) {
        int ctrlChannel = channelSettings->m_nCtrlChannel;

        QString channelName = m_pSenseInterface->getChannelByCtrlChannelNo(ctrlChannel); // sense interface can change name (REF)!
        if(channelName.isEmpty())
            continue;

        qint8 plugBitNo = channelSettings->m_nPluggedBit;
        if(plugBitNo < 0)
            continue;

        quint16 bmask = (1 << plugBitNo);
        if ((clChange & bmask) > 0) {
            if ((m_nClampStatus & bmask) == 0)
                tryAddClamp(channelSettings);
            else
                handleClampDisconnected(channelName, channelSettings, bmask);
        }
    }
}

void cClampInterface::executeProtoScpi(int cmdCode, ProtonetCommandPtr protoCmd)
{
    switch (cmdCode) {
    case ClampSystem::cmdClampChannelCat:
        protoCmd->m_sOutput = readClampChannelCatalog(protoCmd->m_sInput);
        break;
    case ClampSystem::cmdClampWrite:
        protoCmd->m_sOutput = writeAllClamps(protoCmd->m_sInput);
        break;
    case ClampSystem::cmdClampImportExport:
        protoCmd->m_sOutput = importExportAllClamps(protoCmd->m_sInput);
        break;
    }
    if (protoCmd->m_bwithOutput) {
        emit cmdExecutionDone(protoCmd);
    }
}

void cClampInterface::generateAndNotifyClampChannelList()
{
    QStringList clampList = m_clampHash.keys() + m_clampSecondarySet.values();
    m_notifierClampChannelList = clampList.join(";") + ";";
}

QString cClampInterface::readClampChannelCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        generateAndNotifyClampChannelList();
        return m_notifierClampChannelList.getString();
    }
    else {
        return ZSCPI::scpiAnswer[ZSCPI::nak];
    }
}

QString cClampInterface::writeAllClamps(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isCommand(1) && (cmd.getParam(0) == "")) {
        if (m_clampHash.count() > 0) {
            bool enable;
            if (m_ctrlFactory->getPermissionCheckController()->hasPermission(enable)) {
                if (enable) {
                    bool done = true;
                    QDateTime now = QDateTime::currentDateTime();
                    for(auto clamp : qAsConst(m_clampHash))
                        done = done && clamp->exportClampAdjData(now);
                    if (!done)
                        return ZSCPI::scpiAnswer[ZSCPI::errexec];
                }
                else {
                    return ZSCPI::scpiAnswer[ZSCPI::erraut];
                }
            }
            else {
                return ZSCPI::scpiAnswer[ZSCPI::errexec];
            }
        }
        return ZSCPI::scpiAnswer[ZSCPI::ack]; // we return ack even in case there is no clamp because nothing went wrong
    }
    else {
        return ZSCPI::scpiAnswer[ZSCPI::nak];
    }
}

QString cClampInterface::importClampXmls(QString allXML, bool computeAndExport)
{
    // here we got 1 to n concenated xml document's that we want distribute to connected clamps.
    // if we got more than 1 xml document we first check if we have the correct clamps connected
    // we do this using the serial numbers
    // if we only have 1 clamp and 1 xml document we accept this document and take the data for
    // initialzing. so this function can be used by testing field to set up new clamps :-)
    QString sep = "<!DOCTYPE";
    QStringList sl = allXML.split(sep, Qt::SkipEmptyParts);
    int anzXML = sl.count();
    int anzClampTemp = m_clampHash.count();
    if ( !((anzXML > 0) && (anzClampTemp > 0)) )
        return ZSCPI::scpiAnswer[ZSCPI::errxml];

    for (int i = 0; i < anzXML && anzClampTemp > 0; i++) {
        QDomDocument justqdom("TheDocument");
        QString XML = sep + sl.at(i);
        if (!justqdom.setContent(XML) )
            return ZSCPI::scpiAnswer[ZSCPI::errxml];
        QString serNoFromXml;
        if (cClamp::importXmlForSerialNo(&justqdom, serNoFromXml)) {
            cClamp *pClamp, *pClamp4Use;
            int anzSNR = 0;
            QList<QString> keylist = m_clampHash.keys();
            int anzClamps = keylist.count();
            for (int j = 0; j < anzClamps; j++) {
                pClamp = m_clampHash[keylist.at(j)];
                if (pClamp->getSerial() == serNoFromXml) {
                    pClamp4Use = pClamp;
                    anzSNR++;
                }
            }
            // see huge comment above
            if ( (anzSNR == 1) || ( (anzSNR == 0) && (anzXML == 1) && (anzClamps == 1)) ) {
                // we have 1 matching serial number or one document/clamp
                anzClampTemp--;
                if (anzSNR == 0) {
                    pClamp4Use = pClamp;
                    pClamp4Use->importXMLDocument(&justqdom, true); // if we only have 1 xml and 1 clamp we accept all information
                }
                else
                    pClamp4Use->importXMLDocument(&justqdom, false); // otherwise clamp type cannot be changed
                if(computeAndExport) {
                    m_pSenseInterface->computeSenseAdjData();
                    // then we let it compute its new adjustment coefficients... we simply call senseinterface's compute
                    // command. we compute a little bit to much but this doesn't matter at all
                    if (!pClamp4Use->exportClampAdjData(QDateTime::currentDateTime())) // and then we program the clamp
                        return ZSCPI::scpiAnswer[ZSCPI::errexec];
                }
            }
        }
        else
            return ZSCPI::scpiAnswer[ZSCPI::errxml];
    }
    return ZSCPI::scpiAnswer[ZSCPI::ack];
}

QString cClampInterface::importExportAllClamps(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return exportXMLString(-1).replace("\n", "");
    else {
        bool enable;
        if (m_ctrlFactory->getPermissionCheckController()->hasPermission(enable)) {
            if (enable) {
                QString allXML = cmd.getParam(); // we fetch all input
                while (allXML[0] == QChar(' ')) // we remove all leading blanks
                    allXML.remove(0,1);
                return importClampXmls(allXML, true);
            }
            else
                return ZSCPI::scpiAnswer[ZSCPI::erraut];
        }
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
        return ZSCPI::scpiAnswer[ZSCPI::ack];
    }
}
