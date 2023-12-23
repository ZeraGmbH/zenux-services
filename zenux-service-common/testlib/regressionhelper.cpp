#include "regressionhelper.h"
#include "protobufscpitestclient.h"
#include <QJsonObject>

static const QString JsonAvailStr = QStringLiteral("avail");
static const QString JsonUrValStr = QStringLiteral("urval");
static const QString JsonRejectionStr = QStringLiteral("rejection");
static const QString JsonOvRejectionStr = QStringLiteral("ovrejection");
static const QString JsonAdcRejectionStr = QStringLiteral("adcrejection");

void RegressionHelper::addRangeConstantDataToJson(QString rangeName, SenseSystem::cChannelSettings *channelSettings, QJsonObject &range)
{
    QString channelName = channelSettings->m_nameMx;

    // stolen from cPCBInterfacePrivate
    QString avail = ProtobufScpiTestClient::query(QString("SENS:%1:%2:AVA?").arg(channelName, rangeName));
    range.insert(JsonAvailStr, avail);

    QString urValue = ProtobufScpiTestClient::query(QString("SENS:%1:%2:URV?").arg(channelName, rangeName));
    range.insert(JsonUrValStr, urValue);

    QString rejection = ProtobufScpiTestClient::query(QString("SENS:%1:%2:REJ?").arg(channelName, rangeName));
    range.insert(JsonRejectionStr, rejection);

    QString ovRejection = ProtobufScpiTestClient::query(QString("SENS:%1:%2:OVR?").arg(channelName, rangeName));
    range.insert(JsonOvRejectionStr, ovRejection);

    QString adcRejection = ProtobufScpiTestClient::query(QString("SENS:%1:%2:ADCR?").arg(channelName, rangeName));
    range.insert(JsonAdcRejectionStr, adcRejection);
}

bool RegressionHelper::compareRangeConstantDataWithJson(QJsonObject &rangeReference, QString clampName, QString rangeName, SenseSystem::cChannelSettings *channelSettings)
{
    QString channelName = channelSettings->m_nameMx;
    bool allOk = !rangeReference.isEmpty();
    if(allOk) {
        QString avail = ProtobufScpiTestClient::query(QString("SENS:%1:%2:AVA?").arg(channelName, rangeName));
        QString expected = rangeReference.value(JsonAvailStr).toString();
        if(avail != expected) {
            reportError(clampName, rangeName, JsonAvailStr, expected, avail);
            allOk = false;
        }

        QString urValue = ProtobufScpiTestClient::query(QString("SENS:%1:%2:URV?").arg(channelName, rangeName));
        expected = rangeReference.value(JsonUrValStr).toString();
        if(urValue != expected) {
            reportError(clampName, rangeName, JsonUrValStr, expected, urValue);
            allOk = false;
        }

        QString rejection = ProtobufScpiTestClient::query(QString("SENS:%1:%2:REJ?").arg(channelName, rangeName));
        expected = rangeReference.value(JsonRejectionStr).toString();
        if(rejection != expected) {
            reportError(clampName, rangeName, JsonRejectionStr, expected, rejection);
            allOk = false;
        }

        QString ovRejection = ProtobufScpiTestClient::query(QString("SENS:%1:%2:OVR?").arg(channelName, rangeName));
        expected = rangeReference.value(JsonOvRejectionStr).toString();
        if(ovRejection != expected) {
            reportError(clampName, rangeName, JsonOvRejectionStr, expected, ovRejection);
            allOk = false;
        }

        QString adcRejection = ProtobufScpiTestClient::query(QString("SENS:%1:%2:ADCR?").arg(channelName, rangeName));
        expected = rangeReference.value(JsonAdcRejectionStr).toString();
        if(adcRejection != expected) {
            reportError(clampName, rangeName, JsonAdcRejectionStr, expected, adcRejection);
            allOk = false;
        }
    }
    else
        qCritical("No reference data found for clamp \"%s\" / range \"%s\"",
                  qPrintable(clampName),
                  qPrintable(rangeName));
    return allOk;
}

void RegressionHelper::reportError(QString clampName, QString range, QString entry, QString expected, QString found)
{
    qCritical("Clamp: \"%s\" / Range: \"%s\" / Entry: \"%s\": Constant incorrect. Expected: \"%s\" / Found: \"%s\"",
              qPrintable(clampName),
              qPrintable(range),
              qPrintable(entry),
              qPrintable(expected),
              qPrintable(found));
}
