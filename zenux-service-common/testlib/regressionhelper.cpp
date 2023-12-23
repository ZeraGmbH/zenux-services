#include "regressionhelper.h"
#include "protobufscpitestclient.h"
#include <QJsonObject>

void RegressionHelper::addRangeConstantDataToJson(QString rangeName, SenseSystem::cChannelSettings *channelSettings, QJsonObject &range)
{
    QString channelName = channelSettings->m_nameMx;

    // stolen from cPCBInterfacePrivate
    QString avail = ProtobufScpiTestClient::query(QString("SENS:%1:%2:AVA?").arg(channelName, rangeName));
    range.insert("avail", avail);

    QString urValue = ProtobufScpiTestClient::query(QString("SENS:%1:%2:URV?").arg(channelName, rangeName));
    range.insert("urval", urValue);

    QString rejection = ProtobufScpiTestClient::query(QString("SENS:%1:%2:REJ?").arg(channelName, rangeName));
    range.insert("rejection", rejection);

    QString ovRejection = ProtobufScpiTestClient::query(QString("SENS:%1:%2:OVR?").arg(channelName, rangeName));
    range.insert("ovrejection", ovRejection);

    QString adcRejection = ProtobufScpiTestClient::query(QString("SENS:%1:%2:ADCR?").arg(channelName, rangeName));
    range.insert("adcrejection", adcRejection);
}
