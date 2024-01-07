#include "com5003senseinterface.h"
#include "rangeadjustmentinterface.h"
#include "scpiconnection.h"
#include "resource.h"
#include "notzeronumgen.h"
#include "justdatainterface.h"
#include "com5003sensechannel.h"
#include "com5003senserange.h"
#include "protonetcommand.h"
#include "atmel.h"
#include "sensesettings.h"
#include <xmlsettings.h>
#include "zscpi_response_definitions.h"
#include <QList>
#include <QStringList>
#include <QDomElement>
#include <QDomDocument>
#include <QDomText>
#include <QDebug>

Com5003SenseInterface::Com5003SenseInterface(cSCPI *scpiInterface,
                                             RMConnection* rmConnection,
                                             EthSettings *ethSettings,
                                             cSenseSettings *senseSettings,
                                             AtmelPermissionTemplate *permissionQueryHandler) :
    cResource(scpiInterface),
    m_rmConnection(rmConnection),
    m_ethSettings(ethSettings),
    m_permissionQueryHandler(permissionQueryHandler)
{
    m_nMMode = SenseSystem::modeAC; // default ac measurement
    Atmel::getInstance().setMeasMode(m_nMMode); // set the atmels mode too
    setNotifierSenseMMode();

    QList<SenseSystem::cChannelSettings*> channelSettings;
    channelSettings = senseSettings->getChannelSettings();

    // default our sense has 3 voltage and 3 current measuring channels
    Com5003SenseChannel* pChannel;
    pChannel = new Com5003SenseChannel(m_pSCPIInterface, SenseSystem::sVoltageChannelDescription,"V", channelSettings.at(0), 0);
    m_ChannelList.append(pChannel);
    pChannel = new Com5003SenseChannel(m_pSCPIInterface, SenseSystem::sVoltageChannelDescription,"V", channelSettings.at(1), 1);
    m_ChannelList.append(pChannel);
    pChannel = new Com5003SenseChannel(m_pSCPIInterface, SenseSystem::sVoltageChannelDescription,"V", channelSettings.at(2), 2);
    m_ChannelList.append(pChannel);
    pChannel = new Com5003SenseChannel(m_pSCPIInterface, SenseSystem::sCurrentChannelDescription,"A", channelSettings.at(3), 3);
    m_ChannelList.append(pChannel);
    pChannel = new Com5003SenseChannel(m_pSCPIInterface, SenseSystem::sCurrentChannelDescription,"A", channelSettings.at(4), 4);
    m_ChannelList.append(pChannel);
    pChannel = new Com5003SenseChannel(m_pSCPIInterface, SenseSystem::sCurrentChannelDescription,"A", channelSettings.at(5), 5);
    m_ChannelList.append(pChannel);

    QList<Com5003SenseRange*> rngList;

    int i;
    for (i = 0; i < 3; i++)
    {
        rngList.clear();
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,  "480V", true, 480.0, 4712563.0, 5890704.0, 0));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,  "240V", true, 240.0, 4712563.0, 5890704.0, 1));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,  "120V", true, 120.0, 4712563.0, 5890704.0, 2));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,   "60V", true,  60.0, 4712563.0, 5890704.0, 3));

        rngList.append(new Com5003SenseRange(m_pSCPIInterface,   "12V", true,  12.0, 3887864.8, 4859831.0, 4));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,    "5V", true,   5.0, 4516206.0, 5645258.0, 5));

        rngList.append(new Com5003SenseRange(m_pSCPIInterface,   "R0V", false,  9.0, 3839668.2, 5332873.0, 14));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,  "R10V", false, 10.0, 4266298.0, 5332873.0, 15));
        m_ChannelList.at(i)->setRangeList(rngList);
    }

    for (i = 3; i < 6; i++)
    {
        rngList.clear();
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,  "200A", true, 200.0, 6257236.0, 5256077.0, 0 ));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,  "100A", true, 100.0, 4692928.0, 5866160.0, 1 ));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,   "50A", true,  50.0, 4692928.0, 5866160.0, 2 ));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,   "25A", true, 25.0 , 4692928.0, 5866160.0, 3 ));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,   "10A", true, 10.0 , 4692928.0, 5866160.0, 4 ));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,    "5A", true,  5.0 , 4692928.0, 5866160.0, 5 ));

        rngList.append(new Com5003SenseRange(m_pSCPIInterface,  "2.5A", true, 2.5  , 4692928.0, 5866160.0, 6 ));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,  "1.0A", true, 1.0  , 4692928.0, 5866160.0, 7 ));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface, "500mA", true, 0.5  , 4692928.0, 5866160.0, 8 ));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface, "250mA", true, 0.25 , 4692928.0, 5866160.0, 9 ));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface, "100mA", true, 0.1  , 4692928.0, 5866160.0, 10 ));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,  "50mA", true, 0.05 , 4692928.0, 5866160.0, 11));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,  "25mA", true, 0.025, 4692928.0, 5866160.0, 12));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,  "10mA", true, 0.01 , 4692928.0, 5866160.0, 13));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,   "5mA", true, 0.005, 4692928.0, 5866160.0, 14));

        rngList.append(new Com5003SenseRange(m_pSCPIInterface,   "R0V", false,  9.0, 3839668.2, 5332873.0, 15));
        rngList.append(new Com5003SenseRange(m_pSCPIInterface,  "R10V", false, 10.0, 4266298.0, 5332873.0, 16));

        m_ChannelList.at(i)->setRangeList(rngList);
    }

    setNotifierSenseChannelCat(); // only prepared for !!! since we don't have hot plug for measuring channels yet
    m_sVersion = SenseSystem::Version;

    // we set up our statemachine for changing sense mode
    // we must use a statemachine because we have to synchronize sending of notifier
    // otherwise moduls using this notifier will crash because resources are not registered properly

    m_UnregisterSenseState.addTransition(this, SIGNAL(registerRdy()), &m_RegisterSenseState);
    m_RegisterSenseState.addTransition(this, SIGNAL(registerRdy()), &m_NotifySenseState);
    m_ChangeSenseModeMachine.addState(&m_UnregisterSenseState);
    m_ChangeSenseModeMachine.addState(&m_RegisterSenseState);
    m_ChangeSenseModeMachine.addState(&m_NotifySenseState);
    m_ChangeSenseModeMachine.setInitialState(&m_UnregisterSenseState);
    connect(&m_UnregisterSenseState, &QAbstractState::entered, this, &Com5003SenseInterface::unregisterSense);
    connect(&m_RegisterSenseState, &QAbstractState::entered, this, &Com5003SenseInterface::registerSense);
    connect(&m_NotifySenseState, &QAbstractState::entered, this, &Com5003SenseInterface::notifySense);
}


Com5003SenseInterface::~Com5003SenseInterface()
{
    int i;
    Com5003SenseChannel* cptr;

    for (i = 0; i < m_ChannelList.count(); i++)
    {
        cptr = m_ChannelList.at(i);
        delete cptr;
    }
}


void Com5003SenseInterface::initSCPIConnection(QString leadingNodes)
{
    ensureTrailingColonOnNonEmptyParentNodes(leadingNodes);
    addDelegate(QString("%1SENSE").arg(leadingNodes),"VERSION",SCPI::isQuery, m_pSCPIInterface, SenseSystem::cmdVersion);
    addDelegate(QString("%1SENSE").arg(leadingNodes),"MMODE",SCPI::isQuery | SCPI::isCmdwP , m_pSCPIInterface, SenseSystem::cmdMMode, &notifierSenseMMode);
    addDelegate(QString("%1SENSE:MMODE").arg(leadingNodes),"CATALOG",SCPI::isQuery, m_pSCPIInterface, SenseSystem::cmdMModeCat);
    addDelegate(QString("%1SENSE:CHANNEL").arg(leadingNodes),"CATALOG", SCPI::isQuery, m_pSCPIInterface, SenseSystem::cmdChannelCat, &notifierSenseChannelCat);
    addDelegate(QString("%1SENSE:GROUP").arg(leadingNodes),"CATALOG", SCPI::isQuery, m_pSCPIInterface, SenseSystem::cmdGroupCat);
    addDelegate(QString("%1SENSE:CORRECTION").arg(leadingNodes),"INIT", SCPI::isCmd, m_pSCPIInterface, SenseSystem::initAdjData);
    addDelegate(QString("%1SENSE:CORRECTION").arg(leadingNodes),"COMPUTE", SCPI::isCmd, m_pSCPIInterface, SenseSystem::computeAdjData);
    for(auto channel : qAsConst(m_ChannelList)) {
        // we also must connect the signals for notification and for output
        connect(channel, &ScpiConnection::sendNotification, this, &ScpiConnection::sendNotification);
        connect(channel, &ScpiConnection::cmdExecutionDone, this, &ScpiConnection::cmdExecutionDone);
        connect(this, &ScpiConnection::removingSubscribers, channel, &ScpiConnection::onRemoveSubscribers);
        channel->initSCPIConnection(QString("%1SENSE").arg(leadingNodes));
    }
}


Com5003SenseChannel *Com5003SenseInterface::getChannel(QString &name)
{
    int i;

    for (i = 0; i < m_ChannelList.count(); i++)
        if (m_ChannelList.at(i)->getName() == name)
            break;

    if (i < m_ChannelList.count())
        return m_ChannelList.at(i);
    else
        return 0;
}


quint8 Com5003SenseInterface::getAdjustmentStatus()
{
    quint8 adj = 255;
    for (int i = 0; i < m_ChannelList.count(); i++)
        adj &= m_ChannelList.at(i)->getAdjustmentStatus();

    return adj;
}


void Com5003SenseInterface::executeProtoScpi(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case SenseSystem::cmdVersion:
        protoCmd->m_sOutput = scpiReadVersion(protoCmd->m_sInput);
        if (protoCmd->m_bwithOutput)
            emit cmdExecutionDone(protoCmd);
        break;
    case SenseSystem::cmdMMode:
        scpiReadWriteMMode(protoCmd);
        // we have to start statemachine when setting
        break;
    case SenseSystem::cmdMModeCat:
        protoCmd->m_sOutput = m_ReadMModeCatalog(protoCmd->m_sInput);
        if (protoCmd->m_bwithOutput)
            emit cmdExecutionDone(protoCmd);
        break;
    case SenseSystem::cmdChannelCat:
        protoCmd->m_sOutput = m_ReadSenseChannelCatalog(protoCmd->m_sInput);
        if (protoCmd->m_bwithOutput)
            if (protoCmd->m_bwithOutput)
                emit cmdExecutionDone(protoCmd);
        break;
    case SenseSystem::cmdGroupCat:
        protoCmd->m_sOutput = m_ReadSenseGroupCatalog(protoCmd->m_sInput);
        if (protoCmd->m_bwithOutput)
            emit cmdExecutionDone(protoCmd);
        break;
    case SenseSystem::initAdjData:
        protoCmd->m_sOutput = m_InitSenseAdjData(protoCmd->m_sInput);
        if (protoCmd->m_bwithOutput)
            emit cmdExecutionDone(protoCmd);
        break;
    case SenseSystem::computeAdjData:
        protoCmd->m_sOutput = m_ComputeSenseAdjData(protoCmd->m_sInput);
        if (protoCmd->m_bwithOutput)
            emit cmdExecutionDone(protoCmd);
        break;
        break;
    }


}


bool Com5003SenseInterface::importAdjData(QString &s, QDataStream &stream)
{
    QStringList spec;
    spec = s.split(':');
    if (spec.at(0) == "SENSE" )
    {
        Com5003SenseChannel* chn;
        QString s = spec.at(1);
        if ((chn = getChannel(s)) != 0)
        {
            Com5003SenseRange* rng;
            s = spec.at(2);
            if ((rng = chn->getRange(s)) != 0)
            {
                rng->getJustData()->Deserialize(stream);
                return true;
            }
        }

        RangeAdjustmentInterface dummy(m_pSCPIInterface, AdjustScpiValueFormatterFactory::createCom5003AdjFormatter()); // if the data was for SENSE but we didn't find channel or range
        dummy.Deserialize(stream); // we read the data from stream to keep it in flow
        return true;
    }
    else
        return false;
}


void Com5003SenseInterface::exportAdjData(QDataStream &stream, QDateTime dateTimeWrite)
{
    Q_UNUSED(dateTimeWrite)
    for (int i = 0; i < m_ChannelList.count(); i++)
    {
        QList<Com5003SenseRange*> list = m_ChannelList.at(i)->getRangeList();
        QString spec;

        for (int j = 0; j < list.count(); j ++)
        {
             spec = QString("%1:%2:%3")
                 .arg("SENSE")
                 .arg(m_ChannelList.at(i)->getName())
                 .arg(list.at(j)->getName());

             stream << spec.toLatin1();
             list.at(j)->getJustData()->Serialize(stream);
        }
    }
}


void Com5003SenseInterface::exportAdjDataXml(QDomDocument& doc, QDomElement& adjtag)
{
    QDomElement typeTag = doc.createElement("Sense");
    adjtag.appendChild(typeTag);
    for (int i = 0; i < m_ChannelList.count(); i ++) {
        QDomText t;
        QDomElement chtag = doc.createElement("Channel");
        typeTag.appendChild( chtag );
        QDomElement nametag = doc.createElement("Name");
        chtag.appendChild(nametag);
        t = doc.createTextNode(m_ChannelList.at(i)->getName());
        nametag.appendChild( t );

        QList<Com5003SenseRange*> list = m_ChannelList.at(i)->getRangeList();
        for (int j = 0; j < list.count(); j++) {
            Com5003SenseRange* rng = list.at(j);

            QDomElement rtag = doc.createElement("Range");
            chtag.appendChild(rtag);

            nametag = doc.createElement("Name");
            rtag.appendChild(nametag);

            t = doc.createTextNode(list.at(j)->getName());
            nametag.appendChild( t );

            QDomElement gpotag;
            const QStringList listAdjTypes = QStringList() << "Gain" << "Phase" << "Offset";
            for(const auto &adjType : listAdjTypes) {
                gpotag = doc.createElement(adjType);
                rtag.appendChild(gpotag);
                JustDataInterface* adjDataInterface = rng->getJustData()->getAdjInterface(adjType);
                QDomElement tag = doc.createElement("Status");
                QString jdata = adjDataInterface->SerializeStatus();
                t = doc.createTextNode(jdata);
                gpotag.appendChild(tag);
                tag.appendChild(t);
                tag = doc.createElement("Coefficients");
                gpotag.appendChild(tag);
                jdata = adjDataInterface->SerializeCoefficients();
                t = doc.createTextNode(jdata);
                tag.appendChild(t);
                tag = doc.createElement("Nodes");
                gpotag.appendChild(tag);
                jdata = adjDataInterface->SerializeNodes();
                t = doc.createTextNode(jdata);
                tag.appendChild(t);
            }
        }
    }
}


bool Com5003SenseInterface::importAdjDataXml(QDomNode& node) // n steht auf einem element dessen tagname channel ist
{
    if (node.toElement().tagName() != "Sense") // data not for us
        return false;

    QDomNodeList nl=node.childNodes(); // we have a list our channels entries now

    for (qint32 i = 0; i < nl.length(); i++) {
        QDomNode chnNode = nl.item(i); // we iterate over all channels from xml file

        QDomNodeList adjChildNl = chnNode.childNodes();
        Com5003SenseChannel* chnPtr = nullptr;
        Com5003SenseRange* rngPtr = nullptr;
        for (qint32 j = 0; j < adjChildNl.length(); j++) {
            QString Name;
            QDomNode ChannelJustNode = adjChildNl.item(j);
            QDomElement e = ChannelJustNode.toElement();
            QString tName=e.tagName();
            if (tName == "Name") {
                Name = e.text();
                chnPtr = getChannel(Name);
            }
            if (tName == "Range") {
                if (chnPtr != 0) { // if we know this channel
                    QDomNodeList chnJustNl = ChannelJustNode.childNodes();
                    for (qint32 k = 0; k < chnJustNl.length(); k++) {
                        QDomNode RangeJustNode = chnJustNl.item(k);
                        e = RangeJustNode.toElement();
                        tName = e.tagName();
                        if (tName == "Name") {
                            Name = e.text();
                            rngPtr = chnPtr->getRange(Name);
                        }
                        JustDataInterface* pJustData = nullptr;
                        if (rngPtr != nullptr)
                            pJustData = rngPtr->getJustData()->getAdjInterface(tName);
                        if (pJustData) {
                            QDomNodeList jdataNl = RangeJustNode.childNodes();
                            for (qint32 k = 0; k < jdataNl.count(); k++) {
                                QDomNode jTypeNode = jdataNl.item(k);
                                QString jTypeName = jTypeNode.toElement().tagName();
                                QString jdata = jTypeNode.toElement().text();
                                if (jTypeName == "Status")
                                    pJustData->DeserializeStatus(jdata);

                                if (jTypeName == "Coefficients")
                                    pJustData->DeserializeCoefficients(jdata);

                                if (jTypeName == "Nodes")
                                    pJustData->DeserializeNodes(jdata);
                            }
                        }
                    }
                }
            }
        }
    }

    return true;
}


void Com5003SenseInterface::registerResource(RMConnection *rmConnection, quint16 port)
{
    Com5003SenseChannel* pChannel;
    msgNrList.clear();
    for (int i = 0; i < 6; i++)
    {
        pChannel = m_ChannelList.at(i);
        register1Resource(rmConnection, NotZeroNumGen::getMsgNr(), QString("SENSE;%1;1;%2;%3;")
                         .arg(pChannel->getName())
                         .arg(pChannel->getDescription())
                         .arg(port));
    }

    // additional we register measuring mode switch as resource
    register1Resource(rmConnection, NotZeroNumGen::getMsgNr(), QString("SENSE;MMODE;1;%1;%2;")
                      .arg(SenseSystem::sMeasuringModeDescription)
                      .arg(port));

}

void Com5003SenseInterface::computeSenseAdjData()
{
    for(auto channel : qAsConst(m_ChannelList)) {
        channel->computeJustData();
    }
}


QString Com5003SenseInterface::scpiReadVersion(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return m_sVersion;
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}


void Com5003SenseInterface::scpiReadWriteMMode(cProtonetCommand *protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;
    if (cmd.isQuery()) {
        //return SenseSystem::sMMode[m_nMMode];
        protoCmd->m_sOutput  = notifierSenseMMode.getString();
        if (protoCmd->m_bwithOutput)
            emit cmdExecutionDone(protoCmd);
    }
    else {
        if (cmd.isCommand(1)) {
            QString mode = cmd.getParam(0);
            quint8 oldMode = m_nMMode;
            if (mode == SenseSystem::sMMode[SenseSystem::modeAC] ) {
                protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::ack];
                m_nMMode = SenseSystem::modeAC;
                if (oldMode != m_nMMode) {
                    sensemodeProtonetCmdList.append(protoCmd);
                    m_ChangeSenseModeMachine.start(); // we emit cmdExecutionDone when statemachine is finished
                }
                else
                    emit cmdExecutionDone(protoCmd);
            }
            else if (mode == SenseSystem::sMMode[SenseSystem::modeREF] ) {
                protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::ack];
                m_nMMode = SenseSystem::modeREF;
                if (oldMode != m_nMMode) {
                    sensemodeProtonetCmdList.append(protoCmd);
                    m_ChangeSenseModeMachine.start();
                }
                else
                    emit cmdExecutionDone(protoCmd);
            }
            else {
                protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
                if (protoCmd->m_bwithOutput)
                    emit cmdExecutionDone(protoCmd);
            }
        }
        else {
            protoCmd->m_sOutput = ZSCPI::scpiAnswer[ZSCPI::nak];
            if (protoCmd->m_bwithOutput)
                emit cmdExecutionDone(protoCmd);
        }
    }
}

QString Com5003SenseInterface::m_ReadMModeCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        QString s;
        int i;
        for (i = 0; i < SenseSystem::modeAnz-1; i++ )
            s += SenseSystem::sMMode[i] + ";";
        s += SenseSystem::sMMode[i];
        return s;
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString Com5003SenseInterface::m_ReadSenseChannelCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery())
        return notifierSenseChannelCat.getString();
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString Com5003SenseInterface::m_ReadSenseGroupCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if (cmd.isQuery()) {
        QString s;
        if ( m_nMMode == SenseSystem::modeAC )
            s = ";";
        else
            s = "m0,m1,m2;m3,m4,m5;";
        return s;
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString Com5003SenseInterface::m_InitSenseAdjData(QString &sInput)
{
    cSCPICommand cmd = sInput;
    // cmd.isCommand(0) is not correct but we leave it for compatibility
    if ( cmd.isCommand(0) || (cmd.isCommand(1) && (cmd.getParam(0) == ""))) {
        bool enable;
        if (m_permissionQueryHandler->hasPermission(enable)) {
            if (enable) {
                for (int i = 0; i < m_ChannelList.count(); i++)
                    m_ChannelList.at(i)->initJustData();

                return ZSCPI::scpiAnswer[ZSCPI::ack];
            }
            else
                return ZSCPI::scpiAnswer[ZSCPI::erraut];
        }
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

QString Com5003SenseInterface::m_ComputeSenseAdjData(QString &sInput)
{
    cSCPICommand cmd = sInput;
    if ( cmd.isCommand(1) && (cmd.getParam(0) == "") ) {
        bool enable;
        if (m_permissionQueryHandler->hasPermission(enable)) {
            if (enable) {
                for (int i = 0; i < m_ChannelList.count(); i++)
                    m_ChannelList.at(i)->computeJustData();
                return ZSCPI::scpiAnswer[ZSCPI::ack];
            }
            else
               return ZSCPI::scpiAnswer[ZSCPI::erraut];
        }
        else
            return ZSCPI::scpiAnswer[ZSCPI::errexec];
    }
    else
        return ZSCPI::scpiAnswer[ZSCPI::nak];
}

void Com5003SenseInterface::setNotifierSenseMMode()
{
    notifierSenseMMode = SenseSystem::sMMode[m_nMMode];
}

void Com5003SenseInterface::setNotifierSenseChannelCat()
{
    int i;
    QString s;
    for (i = 0; i < m_ChannelList.count()-1; i++ )
        s += m_ChannelList.at(i)->getName() + ";";
    s += m_ChannelList.at(i)->getName();
    notifierSenseChannelCat = s;
}

void Com5003SenseInterface::unregisterSense()
{
    Com5003SenseChannel* pChannel;
    msgNrList.clear();
    for (int i = 0; i < 6; i++) {
        pChannel = m_ChannelList.at(i);
        unregister1Resource(m_rmConnection, NotZeroNumGen::getMsgNr(), QString("SENSE;%1;")
                                                                         .arg(pChannel->getName()));
    }
}

void Com5003SenseInterface::registerSense()
{
    QString s;
    // first we change the channels units and descriptions
    if (m_nMMode == SenseSystem::modeAC) {
        m_ChannelList.at(0)->setDescription(SenseSystem::sVoltageChannelDescription);
        m_ChannelList.at(0)->setUnit(s = "V");
        m_ChannelList.at(1)->setDescription(SenseSystem::sVoltageChannelDescription);
        m_ChannelList.at(1)->setUnit(s = "V");
        m_ChannelList.at(2)->setDescription(SenseSystem::sVoltageChannelDescription);
        m_ChannelList.at(2)->setUnit(s = "V");

        m_ChannelList.at(3)->setDescription(SenseSystem::sCurrentChannelDescription);
        m_ChannelList.at(3)->setUnit(s = "A");
        m_ChannelList.at(4)->setDescription(SenseSystem::sCurrentChannelDescription);
        m_ChannelList.at(4)->setUnit(s = "A");
        m_ChannelList.at(5)->setDescription(SenseSystem::sCurrentChannelDescription);
        m_ChannelList.at(5)->setUnit(s = "A");
    }
    else {
        for (qint32 i = 0; i < m_ChannelList.count(); i++) // for each channel
        {
            m_ChannelList.at(i)->setDescription(SenseSystem::sReferenceChannelDescription);
            m_ChannelList.at(i)->setUnit(s = "V");
        }
    }

    Atmel::getInstance().setMeasMode(m_nMMode); // set the atmels mode too

    // here we do the rest of reconfiguring
    for (qint32 i = 0; i < m_ChannelList.count(); i++) {
        m_ChannelList.at(i)->setMMode(m_nMMode); // this indirectly changes the channnels alias
        QList<Com5003SenseRange*> list = m_ChannelList.at(i)->getRangeList();
        for (int j = 0; j < list.count(); j++ )
            list.at(j)->setAvail( !list.at(j)->getAvail()); // we only toggle the ranges avail

    }
    registerResource(m_rmConnection, m_ethSettings->getPort(EthSettings::protobufserver));
}

void Com5003SenseInterface::notifySense()
{
    setNotifierSenseMMode(); // we set the notifier synchron after all resources are registered again
    cProtonetCommand *protoCmd;
    protoCmd = sensemodeProtonetCmdList.takeFirst();
    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}
