#include "clampinterface.h"
#include "clamp.h"
#include "mt310s2senseinterface.h"
#include "micro-controller-io/atmel.h"
#include "i2csettings.h"
#include "zscpi_response_definitions.h"
#include <i2cmultiplexerfactory.h>
#include <i2cutils.h>
#include <i2cmuxerscopedonoff.h>

cClampInterface::cClampInterface(cPCBServer *server, cI2CSettings *i2cSettings, cSenseSettings *senseSettings, Mt310s2SenseInterface *senseInterface) :
    ScpiConnection(server->getSCPIInterface()),
    m_pMyServer(server),
    m_i2cSettings(i2cSettings),
    m_senseSettings(senseSettings),
    m_pSenseInterface(senseInterface)
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

void cClampInterface::handleClampConnected(QString channelName, const SenseSystem::cChannelSettings *chSettings, quint16 bmask, int phaseCount)
{
    int ctrlChannel = chSettings->m_nCtrlChannel;
    I2cMuxerInterface::Ptr i2cMuxer = I2cMultiplexerFactory::createPCA9547Muxer(m_i2cSettings->getDeviceNode(),
                                                                                m_i2cSettings->getI2CAdress(i2cSettings::muxerI2cAddress),
                                                                                chSettings->m_nMuxChannelNo);
    I2cMuxerScopedOnOff i2cMuxOnOff(i2cMuxer);
    QString i2cDevNode = m_i2cSettings->getDeviceNode();
    int i2cAddress = m_i2cSettings->getI2CAdress(i2cSettings::clampFlashI2cAddress);
    if(I2cPing(i2cDevNode, i2cAddress)) { // ignore other than flash
        m_nClampStatus |= bmask;
        int ctlChannelSecondary = ctrlChannel-phaseCount; // assumption - hope we find better
        cClamp* clamp = new cClamp(m_pMyServer, m_i2cSettings, m_pSenseInterface, channelName, ctrlChannel, i2cMuxer, ctlChannelSecondary);
        m_clampHash[channelName] = clamp;
        qInfo("Add clamp channel \"%s\"/%i", qPrintable(channelName), ctrlChannel);
        QString channelNameSecondary = m_pSenseInterface->getChannelSystemName(ctlChannelSecondary);
        if(!m_clampHash[channelName]->getChannelNameSecondary().isEmpty()) {
            m_clampSecondarySet.insert(channelNameSecondary);
            qInfo("Added voltage clamp channel \"%s\"/%i", qPrintable(channelNameSecondary), ctlChannelSecondary);
        }
        generateAndNotifyClampChannelList();
    }
    else
        qInfo("Not a clamp channel \"%s\"/%i", qPrintable(channelName), ctrlChannel);
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
                handleClampConnected(channelName, channelSettings, bmask, channelsSettings.size()/2);
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
            if (Atmel::getInstance().hasPermission(enable)) {
                if (enable) {
                    bool done = true;
                    for(auto clamp : qAsConst(m_clampHash))
                        done = done && clamp->exportAdjFlash();
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

QString cClampInterface::importExportAllClamps(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        QString xmlTotal;
        for(auto clamp : qAsConst(m_clampHash)) {
            QString xmlClamp = clamp->exportXMLString(-1);
            xmlClamp.replace("\n","");
            xmlTotal.append(xmlClamp);
        }
        return xmlTotal;
    }
    else {
        // here we got 1 to n concenated xml document's that we want distribute to connected clamps.
        // if we got more than 1 xml document we first check if we have the correct clamps connected
        // we do this using the serial numbers
        // if we only have 1 clamp and 1 xml document we accept this document and take the data for
        // initialzing. so this function can be used by testing field to set up new clamps :-)
        QString answer;
        bool err = false;
        bool enable;
        if (Atmel::getInstance().hasPermission(enable)) {
            if (enable) {
                QString allXML = cmd.getParam(); // we fetch all input
                while (allXML[0] == QChar(' ')) { // we remove all leading blanks
                    allXML.remove(0,1);
                }
                QString sep = "<!DOCTYPE";
                QStringList sl = allXML.split(sep, Qt::SkipEmptyParts);
                int anzXML = sl.count();
                int anzClampTemp = m_clampHash.count();
                if ( !((anzXML > 0) && (anzClampTemp > 0)) ) {
                    err = true;
                    answer = ZSCPI::scpiAnswer[ZSCPI::errxml];
                }
                if (!err) {
                    for (int i = 0; (i < anzXML) && (anzClampTemp > 0); i++) {
                        QString XML;
                        QDomDocument justqdom( "TheDocument" );
                        XML = sep + sl.at(i);
                        if ( !justqdom.setContent(XML) ) {
                            err = true;
                            answer = ZSCPI::scpiAnswer[ZSCPI::errxml];
                            break;
                        }
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
                                else {
                                    pClamp4Use->importXMLDocument(&justqdom, false); // otherwise clamp type cannot be changed
                                }
                                m_pSenseInterface->m_ComputeSenseAdjData();
                                // then we let it compute its new adjustment coefficients... we simply call senseinterface's compute
                                // command. we compute a little bit to much but this doesn't matter at all
                                if (!pClamp4Use->exportAdjFlash()) {// and then we program the clamp
                                    err = true;
                                    answer = ZSCPI::scpiAnswer[ZSCPI::errexec];
                                    break;
                                }
                            }
                        }
                        else {
                            err = true;
                            answer = ZSCPI::scpiAnswer[ZSCPI::errxml];
                            break;
                        }
                    }
                }
            }
            else {
                err = true;
                answer = ZSCPI::scpiAnswer[ZSCPI::erraut];
            }
        }
        else {
            err = true;
            answer = ZSCPI::scpiAnswer[ZSCPI::errexec];
        }
        if (!err) {
            answer = ZSCPI::scpiAnswer[ZSCPI::ack];
        }
        return answer;
    }
}
