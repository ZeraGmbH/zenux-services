#include <QList>
#include <QStringList>
#include <QDomElement>
#include <QDomDocument>
#include <QDomText>
#include <QDebug>
#include <scpiconnection.h>
#include "resource.h"

#include "senseinterface.h"
#include "com5003d.h"
#include "justdata.h"
#include "sensechannel.h"
#include "senserange.h"
#include "adjflash.h"
#include <protonetcommand.h>
#include "micro-controller-io/atmel.h"
#include "settings/ethsettings.h"
#include "settings/sensesettings.h"
#include <xmlsettings.h>


extern cATMEL* pAtmel;

cSenseInterface::cSenseInterface(cCOM5003dServer *server) :
    cResource(server->getSCPIInterface())
{
    int i;

    m_pMyServer = server;
    m_nMMode = SenseSystem::modeAC; // default ac measurement
    pAtmel->setMeasMode(m_nMMode); // set the atmels mode too
    setNotifierSenseMMode();

    QList<SenseSystem::cChannelSettings*> channelSettings;
    channelSettings = server->m_pSenseSettings->getChannelSettings();

    // default our sense has 3 voltage and 3 current measuring channels
    cSenseChannel* pChannel;
    pChannel = new cSenseChannel(m_pSCPIInterface, SenseSystem::sVoltageChannelDescription,"V", channelSettings.at(0), 0);
    m_ChannelList.append(pChannel);
    pChannel = new cSenseChannel(m_pSCPIInterface, SenseSystem::sVoltageChannelDescription,"V", channelSettings.at(1), 1);
    m_ChannelList.append(pChannel);
    pChannel = new cSenseChannel(m_pSCPIInterface, SenseSystem::sVoltageChannelDescription,"V", channelSettings.at(2), 2);
    m_ChannelList.append(pChannel);
    pChannel = new cSenseChannel(m_pSCPIInterface, SenseSystem::sCurrentChannelDescription,"A", channelSettings.at(3), 3);
    m_ChannelList.append(pChannel);
    pChannel = new cSenseChannel(m_pSCPIInterface, SenseSystem::sCurrentChannelDescription,"A", channelSettings.at(4), 4);
    m_ChannelList.append(pChannel);
    pChannel = new cSenseChannel(m_pSCPIInterface, SenseSystem::sCurrentChannelDescription,"A", channelSettings.at(5), 5);
    m_ChannelList.append(pChannel);

    // no it doesn't have, the above will be changed if necessary
    // and 6 reference channels
    /*
    pChannel = new cSenseChannel("Reference channel 0..10V DC","V", channelSettings.at(6), 6);
    m_ChannelList.append(pChannel);
    pChannel = new cSenseChannel("Reference channel 0..10V DC","V", channelSettings.at(7), 7);
    m_ChannelList.append(pChannel);
    pChannel = new cSenseChannel("Reference channel 0..10V DC","V", channelSettings.at(8), 8);
    m_ChannelList.append(pChannel);
    pChannel = new cSenseChannel("Reference channel 0..10V DC","V", channelSettings.at(9), 9);
    m_ChannelList.append(pChannel);
    pChannel = new cSenseChannel("Reference channel 0..10V DC","V", channelSettings.at(10), 10);
    m_ChannelList.append(pChannel);
    pChannel = new cSenseChannel("Reference channel 0..10V DC","V", channelSettings.at(11), 11);
    m_ChannelList.append(pChannel);
    */

    QList<cSenseRange*> rngList;

    for (i = 0; i < 3; i++)
    {
        rngList.clear();
        rngList.append(new cSenseRange(m_pSCPIInterface, "480V", "480V", true, 480.0, 4712563.0, 5890704.0, 8388607.0, 0, SenseRange::Phys));
        rngList.append(new cSenseRange(m_pSCPIInterface, "240V", "240V", true, 240.0, 4712563.0, 5890704.0, 8388607.0, 1, SenseRange::Phys));
        rngList.append(new cSenseRange(m_pSCPIInterface, "120V", "120V", true, 120.0, 4712563.0, 5890704.0, 8388607.0, 2, SenseRange::Phys));
        rngList.append(new cSenseRange(m_pSCPIInterface, "60V" , "60V" , true, 60.0 , 4712563.0, 5890704.0, 8388607.0, 3, SenseRange::Phys));

        rngList.append(new cSenseRange(m_pSCPIInterface, "12V" , "12V" , true, 12.0 , 3887864.8, 4859831.0, 8388607.0, 4, SenseRange::Phys));
        rngList.append(new cSenseRange(m_pSCPIInterface, "5V"  , "5V"  , true, 5.0  , 4516206.0, 5645258.0, 8388607.0, 5, SenseRange::Phys));

        rngList.append(new cSenseRange(m_pSCPIInterface, "R0V" , "R0V" , false,  9.0, 3839668.2, 5332873.0, 8388607.0, 14, SenseRange::Phys));
        rngList.append(new cSenseRange(m_pSCPIInterface, "R10V", "R10V", false, 10.0, 4266298.0, 5332873.0, 8388607.0, 15, SenseRange::Phys));
        m_ChannelList.at(i)->setRangeList(rngList);
    }

    for (i = 3; i < 6; i++)
    {
        rngList.clear();
        rngList.append(new cSenseRange(m_pSCPIInterface, "200A" ,"200A" , true, 200.0,6257236.0, 5256077.0, 8388607.0, 0 , SenseRange::Phys));
        rngList.append(new cSenseRange(m_pSCPIInterface, "100A" ,"100A" , true, 100.0,4692928.0, 5866160.0, 8388607.0, 1 , SenseRange::Phys));
        rngList.append(new cSenseRange(m_pSCPIInterface, "50A"  ,"50A"  , true,  50.0,4692928.0, 5866160.0, 8388607.0, 2 , SenseRange::Phys));
        rngList.append(new cSenseRange(m_pSCPIInterface, "25A"  ,"25A"  , true, 25.0 ,4692928.0, 5866160.0, 8388607.0, 3 , SenseRange::Phys));
        rngList.append(new cSenseRange(m_pSCPIInterface, "10A"  ,"10A"  , true, 10.0 ,4692928.0, 5866160.0, 8388607.0, 4 , SenseRange::Phys));
        rngList.append(new cSenseRange(m_pSCPIInterface, "5A"    ,"5A"  , true,  5.0 ,4692928.0, 5866160.0, 8388607.0, 5 , SenseRange::Phys));

        rngList.append(new cSenseRange(m_pSCPIInterface, "2.5A" ,"2.5A" , true, 2.5  ,4692928.0, 5866160.0, 8388607.0, 6 , SenseRange::Phys));
        rngList.append(new cSenseRange(m_pSCPIInterface, "1.0A" ,"1.0A" , true, 1.0  ,4692928.0, 5866160.0, 8388607.0, 7 , SenseRange::Phys));
        rngList.append(new cSenseRange(m_pSCPIInterface, "500mA","500mA", true, 0.5  ,4692928.0, 5866160.0, 8388607.0, 8 , SenseRange::Phys));
        rngList.append(new cSenseRange(m_pSCPIInterface, "250mA","250mA", true, 0.25 ,4692928.0, 5866160.0, 8388607.0, 9 , SenseRange::Phys));
        rngList.append(new cSenseRange(m_pSCPIInterface, "100mA","100mA", true, 0.1  ,4692928.0, 5866160.0, 8388607.0,10 , SenseRange::Phys));
        rngList.append(new cSenseRange(m_pSCPIInterface, "50mA" ,"50mA" , true, 0.05 ,4692928.0, 5866160.0, 8388607.0,11, SenseRange::Phys));
        rngList.append(new cSenseRange(m_pSCPIInterface, "25mA" ,"25mA" , true, 0.025,4692928.0, 5866160.0, 8388607.0,12, SenseRange::Phys));
        rngList.append(new cSenseRange(m_pSCPIInterface, "10mA" ,"10mA" , true, 0.01 ,4692928.0, 5866160.0, 8388607.0,13, SenseRange::Phys));
        rngList.append(new cSenseRange(m_pSCPIInterface, "5mA"  ,"5mA"  , true, 0.005,4692928.0, 5866160.0, 8388607.0,14, SenseRange::Phys));

        rngList.append(new cSenseRange(m_pSCPIInterface, "R0V" , "R0V" , false,  9.0, 3839668.2, 5332873.0, 8388607.0,15, SenseRange::Phys));
        rngList.append(new cSenseRange(m_pSCPIInterface, "R10V", "R10V", false, 10.0, 4266298.0, 5332873.0, 8388607.0,16, SenseRange::Phys));

        m_ChannelList.at(i)->setRangeList(rngList);
    }

    /*
    for (i = 6; i < 12; i++)
    {
        rngList.clear();
        rngList.append(new cSenseRange("0V",10.0,5005789.0, 14, SenseRange::Phys));
        rngList.append(new cSenseRange("10V",10.0,5005789.0, 15, SenseRange::Phys));

        m_ChannelList.at(i)->setRangeList(rngList);
    }
    */

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
    connect(&m_UnregisterSenseState, SIGNAL(entered()), this, SLOT(unregisterSense()));
    connect(&m_RegisterSenseState, SIGNAL(entered()), this, SLOT(registerSense()));
    connect(&m_NotifySenseState, SIGNAL(entered()), this, SLOT(notifySense()));
}


cSenseInterface::~cSenseInterface()
{
    int i;
    cSenseChannel* cptr;

    for (i = 0; i < m_ChannelList.count(); i++)
    {
        cptr = m_ChannelList.at(i);
        delete cptr;
    }
}


void cSenseInterface::initSCPIConnection(QString leadingNodes)
{
    cSCPIDelegate* delegate;

    if (leadingNodes != "")
        leadingNodes += ":";

    delegate = new cSCPIDelegate(QString("%1SENSE").arg(leadingNodes),"VERSION",SCPI::isQuery,m_pSCPIInterface, SenseSystem::cmdVersion);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SENSE").arg(leadingNodes),"MMODE",SCPI::isQuery | SCPI::isCmdwP ,m_pSCPIInterface, SenseSystem::cmdMMode);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SENSE:MMODE").arg(leadingNodes),"CATALOG",SCPI::isQuery,m_pSCPIInterface, SenseSystem::cmdMModeCat );
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SENSE:CHANNEL").arg(leadingNodes),"CATALOG", SCPI::isQuery, m_pSCPIInterface, SenseSystem::cmdChannelCat);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SENSE:GROUP").arg(leadingNodes),"CATALOG", SCPI::isQuery, m_pSCPIInterface, SenseSystem::cmdGroupCat);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));
    delegate = new cSCPIDelegate(QString("%1SENSE:CORRECTION").arg(leadingNodes),"INIT", SCPI::isCmd, m_pSCPIInterface, SenseSystem::initAdjData);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));  
    delegate = new cSCPIDelegate(QString("%1SENSE:CORRECTION").arg(leadingNodes),"COMPUTE", SCPI::isCmd, m_pSCPIInterface, SenseSystem::computeAdjData);
    m_DelegateList.append(delegate);
    connect(delegate, SIGNAL(execute(int, cProtonetCommand*)), this, SLOT(executeCommand(int, cProtonetCommand*)));


    for (int i = 0; i < m_ChannelList.count(); i++)
    {
        // we also must connect the signals for notification and for output
        connect(m_ChannelList.at(i), &cSCPIConnection::strNotifier, this, &cSCPIConnection::strNotifier);
        connect(m_ChannelList.at(i), SIGNAL(cmdExecutionDone(cProtonetCommand*)), this, SIGNAL(cmdExecutionDone(cProtonetCommand*)));

        m_ChannelList.at(i)->initSCPIConnection(QString("%1SENSE").arg(leadingNodes));
    }
}


cSenseChannel *cSenseInterface::getChannel(QString &name)
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


quint8 cSenseInterface::getAdjustmentStatus()
{
    quint8 adj = 255;
    for (int i = 0; i < m_ChannelList.count(); i++)
        adj &= m_ChannelList.at(i)->getAdjustmentStatus();

    return adj;
}


void cSenseInterface::executeCommand(int cmdCode, cProtonetCommand *protoCmd)
{
    switch (cmdCode)
    {
    case SenseSystem::cmdVersion:
        protoCmd->m_sOutput = m_ReadVersion(protoCmd->m_sInput);
        if (protoCmd->m_bwithOutput)
            emit cmdExecutionDone(protoCmd);
        break;
    case SenseSystem::cmdMMode:
        m_ReadWriteMModeVersion(protoCmd);
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


bool cSenseInterface::importAdjData(QString &s, QDataStream &stream)
{
    QStringList spec;
    spec = s.split(':');
    if (spec.at(0) == "SENSE" )
    {
        cSenseChannel* chn;
        QString s = spec.at(1);
        if ((chn = getChannel(s)) != 0)
        {
            cSenseRange* rng;
            s = spec.at(2);
            if ((rng = chn->getRange(s)) != 0)
            {
                rng->getJustData()->Deserialize(stream);
                return true;
            }
        }

        cCOM5003JustData dummy(m_pSCPIInterface); // if the data was for SENSE but we didn't find channel or range
        dummy.Deserialize(stream); // we read the data from stream to keep it in flow
        return true;
    }
    else
        return false;
}


void cSenseInterface::exportAdjData(QDataStream &stream)
{
    for (int i = 0; i < m_ChannelList.count(); i++)
    {
        QList<cSenseRange*> list = m_ChannelList.at(i)->getRangeList();
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


void cSenseInterface::exportAdjData(QDomDocument& doc, QDomElement& qde)
{
    QDomElement typeTag = doc.createElement( "Sense");
    qde.appendChild(typeTag);

    for (int i = 0; i < m_ChannelList.count(); i ++)
    {
        QDomText t;
        QDomElement chtag = doc.createElement( "Channel" );
        typeTag.appendChild( chtag );
        QDomElement nametag = doc.createElement( "Name" );
        chtag.appendChild(nametag);
        t = doc.createTextNode(m_ChannelList.at(i)->getName());
        nametag.appendChild( t );

        QList<cSenseRange*> list = m_ChannelList.at(i)->getRangeList();
        for (int j = 0; j < list.count(); j++)
        {
            cSenseRange* rng = list.at(j);

            QDomElement rtag = doc.createElement( "Range" );
            chtag.appendChild( rtag );

            nametag = doc.createElement( "Name" );
            rtag.appendChild(nametag);

            t = doc.createTextNode(list.at(j)->getName());
            nametag.appendChild( t );

            QDomElement gpotag = doc.createElement( "Gain" );
            rtag.appendChild(gpotag);
            QDomElement tag = doc.createElement( "Status" );
            QString jdata = rng->getJustData()->m_pGainCorrection->SerializeStatus();
            t = doc.createTextNode(jdata);
            gpotag.appendChild(tag);
            tag.appendChild(t);
            tag = doc.createElement( "Coefficients" );
            gpotag.appendChild(tag);
            jdata = rng->getJustData()->m_pGainCorrection->SerializeCoefficients();
            t = doc.createTextNode(jdata);
            tag.appendChild(t);
            tag = doc.createElement( "Nodes" );
            gpotag.appendChild(tag);
            jdata = rng->getJustData()->m_pGainCorrection->SerializeNodes();
            t = doc.createTextNode(jdata);
            tag.appendChild(t);

            gpotag = doc.createElement( "Phase" );
            rtag.appendChild(gpotag);
            tag = doc.createElement( "Status" );
            jdata = rng->getJustData()->m_pPhaseCorrection->SerializeStatus();
            t = doc.createTextNode(jdata);
            tag.appendChild(t);
            gpotag.appendChild(tag);
            tag = doc.createElement( "Coefficients" );
            gpotag.appendChild(tag);
            jdata = rng->getJustData()->m_pPhaseCorrection->SerializeCoefficients();
            t = doc.createTextNode(jdata);
            tag.appendChild(t);
            tag = doc.createElement( "Nodes" );
            gpotag.appendChild(tag);
            jdata = rng->getJustData()->m_pPhaseCorrection->SerializeNodes();
            t = doc.createTextNode(jdata);
            tag.appendChild(t);

            gpotag = doc.createElement( "Offset" );
            rtag.appendChild(gpotag);
            tag = doc.createElement( "Status" );
            jdata = rng->getJustData()->m_pOffsetCorrection->SerializeStatus();
            t = doc.createTextNode(jdata);
            tag.appendChild(t);
            gpotag.appendChild(tag);
            tag = doc.createElement( "Coefficients" );
            gpotag.appendChild(tag);
            jdata = rng->getJustData()->m_pOffsetCorrection->SerializeCoefficients();
            t = doc.createTextNode(jdata);
            tag.appendChild(t);
            tag = doc.createElement( "Nodes" );
            gpotag.appendChild(tag);
            jdata = rng->getJustData()->m_pOffsetCorrection->SerializeNodes();
            t = doc.createTextNode(jdata);
            tag.appendChild(t);
        }
    }
}


bool cSenseInterface::importAdjData(QDomNode& node) // n steht auf einem element dessen tagname channel ist
{
    qDebug() << node.toElement().tagName();
    if (node.toElement().tagName() != "Sense") // data not for us
        return false;

    QDomNodeList nl=node.childNodes(); // we have a list our channels entries now

    for (qint32 i = 0; i < nl.length(); i++)
    {
        QDomNode chnNode = nl.item(i); // we iterate over all channels from xml file

        QDomNodeList nl2 = chnNode.childNodes();
        for (qint32 j = 0; j < nl2.length(); j++)
        {
            cSenseChannel* chnPtr;
            cSenseRange* rngPtr;
            QString Name;

            QDomNode ChannelJustNode = nl2.item(j);
            QDomElement e=ChannelJustNode.toElement();
            QString tName=e.tagName();
            qDebug() << tName;

            if (tName == "Name")
            {
                Name=e.text();
                qDebug() << Name;
                chnPtr = getChannel(Name);
            }

            if (tName == "Range")
            {
                if (chnPtr != 0) // if we know this channel
                {
                    QDomNodeList nl3 = ChannelJustNode.childNodes();

                    for (qint32 k = 0; k < nl3.length(); k++)
                    {
                        QDomNode RangeJustNode = nl3.item(k);

                        e = RangeJustNode.toElement();
                        tName = e.tagName();
                        qDebug() << tName;

                        if (tName == "Name")
                        {
                            Name=e.text();
                            qDebug() << Name;
                            rngPtr = chnPtr->getRange(Name);
                        }

                        cJustData* pJustData = 0;

                        if (rngPtr != 0)
                        {
                            if (tName == "Gain")
                                pJustData = rngPtr->getJustData()->m_pGainCorrection;

                            if (tName == "Phase")
                                pJustData = rngPtr->getJustData()->m_pPhaseCorrection;

                            if (tName == "Offset")
                                pJustData = rngPtr->getJustData()->m_pOffsetCorrection;
                        }

                        if (pJustData)
                        {
                            QDomNodeList nl4 = RangeJustNode.childNodes();
                            for (qint32 k = 0; k < nl4.count(); k++)
                            {
                                QDomNode jTypeNode = nl4.item(k);
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


void cSenseInterface::registerResource(cRMConnection *rmConnection, quint16 port)
{
    cSenseChannel* pChannel;
    msgNrList.clear();
    for (int i = 0; i < 6; i++)
    {
        pChannel = m_ChannelList.at(i);
        register1Resource(rmConnection, m_pMyServer->getMsgNr(), QString("SENSE;%1;1;%2;%3;")
                         .arg(pChannel->getName())
                         .arg(pChannel->getDescription())
                         .arg(port));
    }

    // additional we register measuring mode switch as resource
    register1Resource(rmConnection, m_pMyServer->getMsgNr(), QString("SENSE;MMODE;1;%1;%2;")
                      .arg(SenseSystem::sMeasuringModeDescription)
                      .arg(port));

}


void cSenseInterface::unregisterResource(cRMConnection *rmConnection)
{
    cSenseChannel* pChannel;
    msgNrList.clear();
    for (int i = 0; i < 6; i++)
    {
        pChannel = m_ChannelList.at(i);
        unregister1Resource(rmConnection, m_pMyServer->getMsgNr(), QString("SENSE;%1;")
                         .arg(pChannel->getName()));
    }
}


QString cSenseInterface::m_ReadVersion(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
        return m_sVersion;
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


void cSenseInterface::m_ReadWriteMModeVersion(cProtonetCommand *protoCmd)
{
    cSCPICommand cmd = protoCmd->m_sInput;

    if (cmd.isQuery())
    {
        //return SenseSystem::sMMode[m_nMMode];
        emit strNotifier(&notifierSenseMMode);
        protoCmd->m_sOutput  = notifierSenseMMode.getString();
        if (protoCmd->m_bwithOutput)
            emit cmdExecutionDone(protoCmd);
    }
    else
    {
        if (cmd.isCommand(1))
        {
            QString mode = cmd.getParam(0);

            quint8 oldMode = m_nMMode;

            if (mode == SenseSystem::sMMode[SenseSystem::modeAC] )
            {
                protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::ack];
                m_nMMode = SenseSystem::modeAC;
                if (oldMode != m_nMMode)
                {
                    sensemodeProtonetCmdList.append(protoCmd);
                    m_ChangeSenseModeMachine.start(); // we emit cmdExecutionDone when statemachine is finished
                }
                else
                    emit cmdExecutionDone(protoCmd);


            }

            else

            if (mode == SenseSystem::sMMode[SenseSystem::modeREF] )
            {
                protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::ack];
                m_nMMode = SenseSystem::modeREF;
                if (oldMode != m_nMMode)
                {
                    sensemodeProtonetCmdList.append(protoCmd);
                    m_ChangeSenseModeMachine.start();
                }
                else
                    emit cmdExecutionDone(protoCmd);
            }

            else
            {
                protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];
                if (protoCmd->m_bwithOutput)
                    emit cmdExecutionDone(protoCmd);
            }
        }

        else
        {
            protoCmd->m_sOutput = SCPI::scpiAnswer[SCPI::nak];
            if (protoCmd->m_bwithOutput)
                emit cmdExecutionDone(protoCmd);
        }
    }
}


QString cSenseInterface::m_ReadMModeCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        int i;
        QString s;
        for (i = 0; i < SenseSystem::modeAnz-1; i++ )
            s += SenseSystem::sMMode[i] + ";";
        s += SenseSystem::sMMode[i];
        return s;
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];

}


QString cSenseInterface::m_ReadSenseChannelCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {

        emit strNotifier(&notifierSenseChannelCat);
        return notifierSenseChannelCat.getString();
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cSenseInterface::m_ReadSenseGroupCatalog(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if (cmd.isQuery())
    {
        QString s;
        if ( m_nMMode == SenseSystem::modeAC )
            s = ";";
        else
            s = "m0,m1,m2;m3,m4,m5;";

        return s;
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


QString cSenseInterface::m_InitSenseAdjData(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if ( cmd.isCommand(0) || (cmd.isCommand(1) && (cmd.getParam(0) == "")))
    // cmd.isCommand(0) is not correct but we leave it for compatibility
    {
        bool enable;
        if (pAtmel->getEEPROMAccessEnable(enable) == ZeraMcontrollerBase::cmddone)
        {
            if (enable)
            {
                for (int i = 0; i < m_ChannelList.count(); i++)
                    m_ChannelList.at(i)->initJustData();

                return SCPI::scpiAnswer[SCPI::ack];
            }
            else
                return SCPI::scpiAnswer[SCPI::erraut];
        }
        else
            return SCPI::scpiAnswer[SCPI::errexec];
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];

}


QString cSenseInterface::m_ComputeSenseAdjData(QString &sInput)
{
    cSCPICommand cmd = sInput;

    if ( cmd.isCommand(1) && (cmd.getParam(0) == "") )
    {

        bool enable;
        if (pAtmel->getEEPROMAccessEnable(enable) == ZeraMcontrollerBase::cmddone)
        {
            if (enable)
            {
                for (int i = 0; i < m_ChannelList.count(); i++)
                    m_ChannelList.at(i)->computeJustData();
                return SCPI::scpiAnswer[SCPI::ack];
            }
            else
               return SCPI::scpiAnswer[SCPI::erraut];
        }
        else
            return SCPI::scpiAnswer[SCPI::errexec];
    }
    else
        return SCPI::scpiAnswer[SCPI::nak];
}


void cSenseInterface::setNotifierSenseMMode()
{
    notifierSenseMMode = SenseSystem::sMMode[m_nMMode];
}


void cSenseInterface::setNotifierSenseChannelCat()
{
    int i;
    QString s;
    for (i = 0; i < m_ChannelList.count()-1; i++ )
        s += m_ChannelList.at(i)->getName() + ";";
    s += m_ChannelList.at(i)->getName();
    notifierSenseChannelCat = s;
}


void cSenseInterface::unregisterSense()
{
    unregisterResource(m_pMyServer->m_pRMConnection);
}


void cSenseInterface::registerSense()
{
    QString s;
    qint32 i;

    // first we change the channels units and descriptions
    if (m_nMMode == SenseSystem::modeAC)
    {
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
    else
    {
        for (i = 0; i < m_ChannelList.count(); i++) // for each channel
        {
            m_ChannelList.at(i)->setDescription(SenseSystem::sReferenceChannelDescription);
            m_ChannelList.at(i)->setUnit(s = "V");
        }
    }

    pAtmel->setMeasMode(m_nMMode); // set the atmels mode too

    // here we do the rest of reconfiguring
    for (i = 0; i < m_ChannelList.count(); i++) // for each channel
    {
        m_ChannelList.at(i)->setMMode(m_nMMode); // this indirectly changes the channnels alias
        QList<cSenseRange*> list = m_ChannelList.at(i)->getRangeList();
        for (int j = 0; j < list.count(); j++ )
            list.at(j)->setAvail( !list.at(j)->getAvail()); // we only toggle the ranges avail

    }

    registerResource(m_pMyServer->m_pRMConnection, m_pMyServer->m_pETHSettings->getPort(protobufserver));

}


void cSenseInterface::notifySense()
{
    setNotifierSenseMMode(); // we set the notifier synchron after all resources are registered again
    cProtonetCommand *protoCmd;
    protoCmd = sensemodeProtonetCmdList.takeFirst();
    if (protoCmd->m_bwithOutput)
        emit cmdExecutionDone(protoCmd);
}


/*
void cSenseInterface::registerResource(QDataStream &stream)
{
    for (int i = 0; i < m_ChannelList.count(); i++)
        if (m_ChannelList.at(i)->isAvail() )
            stream << QString("RESOURCE:ADD SENSE;%1;;%2").arg(m_ChannelList.at(i)->getName()
                                                               .arg((m_ChannelList.at(i)->getDescription());
}
*/






