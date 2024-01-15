#include "clampinterface.h"
#include "clampfactory.h"
#include "mt310s2senseinterface.h"
#include "i2csettings.h"
#include "zscpi_response_definitions.h"
#include <i2cmultiplexerfactory.h>
#include <i2cutils.h>
#include <i2cmuxerscopedonoff.h>

cClampInterface::cClampInterface(cPCBServer *server,
                                 cI2CSettings *i2cSettings,
                                 cSenseSettings *senseSettings,
                                 Mt310s2SenseInterface *senseInterface,
                                 FactoryControllerAbstractPtr ctrlFactory) :
    ScpiConnection(server->getSCPIInterface()),
    m_pMyServer(server),
    m_i2cSettings(i2cSettings),
    m_senseSettings(senseSettings),
    m_pSenseInterface(senseInterface),
    m_ctrlFactory(ctrlFactory)
{
    m_nClampStatus = 0;
}

void cClampInterface::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1SYSTEM:CLAMP:CHANNEL").arg(leadingNodes),"CATALOG",SCPI::isQuery, m_pSCPIInterface, ClampSystem::cmdClampChannelCat, &m_notifierClampChannelList);
    addDelegate(QString("%1SYSTEM:CLAMP").arg(leadingNodes),"WRITE",SCPI::isCmd, m_pSCPIInterface, ClampSystem::cmdClampWrite);
    addDelegate(QString("%1SYSTEM:ADJUSTMENT:CLAMP").arg(leadingNodes),"XML",SCPI::isQuery | SCPI::isCmdwP, m_pSCPIInterface, ClampSystem::cmdClampImportExport);
}

void cClampInterface::addClamp(const SenseSystem::cChannelSettings *chSettings, I2cMuxerInterface::Ptr i2cMuxer)
{
    m_nClampStatus |= (1<<chSettings->m_nPluggedBit);

    // assumption - hope we find better
    int phaseCount = m_senseSettings->getChannelSettings().size()/2;
    int ctlChannelSecondary = chSettings->m_nCtrlChannel - phaseCount;

    cClamp* clamp = ClampFactory::createClamp(m_pMyServer, m_i2cSettings, m_pSenseInterface, chSettings->m_nameMx, chSettings->m_nCtrlChannel, i2cMuxer, ctlChannelSecondary);
    m_clampHash[chSettings->m_nameMx] = clamp;
    qInfo("Add clamp ranges for \"%s\"", qPrintable(chSettings->m_sAlias1));
    QString channelNameSecondary = clamp->getChannelNameSecondary();
    if(!m_clampHash[chSettings->m_nameMx]->getChannelNameSecondary().isEmpty()) {
        m_clampSecondarySet.insert(channelNameSecondary);
        qInfo("Add secondary clamp ranges for \"%s\"", qPrintable(m_senseSettings->findChannelSettingByMxName(channelNameSecondary)->m_sAlias1));
    }
    generateAndNotifyClampChannelList();
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

void cClampInterface::handleClampConnected(const SenseSystem::cChannelSettings *chSettings)
{
    QString i2cDevNode = m_i2cSettings->getDeviceNode();
    I2cMuxerInterface::Ptr i2cMuxer = I2cMultiplexerFactory::createPCA9547Muxer(i2cDevNode,
                                                                                m_i2cSettings->getI2CAdress(i2cSettings::muxerI2cAddress),
                                                                                chSettings->m_nMuxChannelNo);
    I2cMuxerScopedOnOff i2cMuxOnOff(i2cMuxer);
    int i2cAddress = m_i2cSettings->getI2CAdress(i2cSettings::clampFlashI2cAddress);
    if(I2cPing(i2cDevNode, i2cAddress)) // ignore other than flash
        addClamp(chSettings, i2cMuxer);
    else
        qInfo("Not a clamp on %s", qPrintable(chSettings->m_sAlias1));
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

        QString channelName = m_pSenseInterface->getChannelSystemName(ctrlChannel); // sense interface can change name (REF)!
        if(channelName.isEmpty())
            continue;

        qint8 plugBitNo = channelSettings->m_nPluggedBit;
        if(plugBitNo < 0)
            continue;

        quint16 bmask = (1 << plugBitNo);
        if ((clChange & bmask) > 0) {
            if ((m_nClampStatus & bmask) == 0)
                handleClampConnected(channelSettings);
            else
                handleClampDisconnected(channelName, channelSettings, bmask);
        }
    }
}

void cClampInterface::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
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
                        done = done && clamp->exportAdjFlash(now);
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
        cClamp tmpClamp;
        if (tmpClamp.importXMLDocument(&justqdom,true)) {
            cClamp *pClamp, *pClamp4Use;
            int anzSNR = 0;
            QList<QString> keylist = m_clampHash.keys();
            int anzClamps = keylist.count();
            for (int j = 0; j < anzClamps; j++) {
                pClamp = m_clampHash[keylist.at(j)];
                if (pClamp->getSerial() == tmpClamp.getSerial()) {
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
                    if (!pClamp4Use->exportAdjFlash(QDateTime::currentDateTime())) // and then we program the clamp
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
