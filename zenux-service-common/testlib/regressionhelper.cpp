#include "regressionhelper.h"
#include "protobufscpitestclient.h"
#include <timemachineobject.h>
#include <QSignalSpy>
#include <QJsonDocument>
#include <QJsonObject>

static const QString JsonAliasStr = QStringLiteral("alias");
static const QString JsonAvailStr = QStringLiteral("avail");
static const QString JsonUrValStr = QStringLiteral("urval");
static const QString JsonRejectionStr = QStringLiteral("rejection");
static const QString JsonOvRejectionStr = QStringLiteral("ovrejection");
static const QString JsonAdcRejectionStr = QStringLiteral("adcrejection");
static const QString JsonAdjustStatusFlags = QStringLiteral("adjuststatusflags");

QString RegressionHelper::getJsonNumString(int clampTypeNo)
{
    return "clamp_type_no_" + (QString("0000") + QString("%1").arg(clampTypeNo)).right(4);
}

void RegressionHelper::addRangeConstantDataToJson(QString rangeName, SenseSystem::cChannelSettings *channelSettings, QJsonObject &range)
{
    QString channelName = channelSettings->m_nameMx;

    // stolen from cPCBInterfacePrivate
    QString alias = ProtobufScpiTestClient::query(QString("SENS:%1:%2:ALI?").arg(channelName, rangeName));
    range.insert(JsonAliasStr, alias);

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

    QString adjustStatusFlags = ProtobufScpiTestClient::query(QString("SENS:%1:%2:TYPE?").arg(channelName, rangeName));
    range.insert(JsonAdjustStatusFlags, adjustStatusFlags);
}

bool RegressionHelper::compareRangeConstantDataWithJson(QJsonObject &rangeReference, QString clampName, QString rangeName, SenseSystem::cChannelSettings *channelSettings)
{
    QString channelName = channelSettings->m_nameMx;
    bool allOk = !rangeReference.isEmpty();
    if(allOk) {
        QString alias = ProtobufScpiTestClient::query(QString("SENS:%1:%2:ALI?").arg(channelName, rangeName));
        QString expected = rangeReference.value(JsonAliasStr).toString();
        if(alias != expected) {
            reportError(clampName, rangeName, JsonAliasStr, expected, alias);
            allOk = false;
        }

        QString avail = ProtobufScpiTestClient::query(QString("SENS:%1:%2:AVA?").arg(channelName, rangeName));
        expected = rangeReference.value(JsonAvailStr).toString();
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

        QString adjustStatusFlags = ProtobufScpiTestClient::query(QString("SENS:%1:%2:TYPE?").arg(channelName, rangeName));
        expected = rangeReference.value(JsonAdjustStatusFlags).toString();
        if(adjustStatusFlags != expected) {
            reportError(clampName, rangeName, JsonAdjustStatusFlags, expected, adjustStatusFlags);
            allOk = false;
        }
    }
    else
        qCritical("No reference data found for clamp \"%s\" / range \"%s\"",
                  qPrintable(clampName),
                  qPrintable(rangeName));
    return allOk;
}

QString RegressionHelper::noClampJsonId = QStringLiteral("no-clamps");

void RegressionHelper::genJsonConstantValuesAllRanges(SenseSystem::cChannelSettings *channelSetting, Zera::cPCBInterface* pcbIFace)
{
    QJsonObject jsonAll;

    QSignalSpy responseSpy(pcbIFace, &Zera::cPCBInterface::serverAnswer);
    pcbIFace->getRangeList(channelSetting->m_nameMx);
    TimeMachineObject::feedEventLoop();

    QJsonObject jsonRanges;
    const QStringList ranges = responseSpy[0][2].toStringList();
    for(const QString &range : ranges) {
        QJsonObject jsonRange;
        RegressionHelper::addRangeConstantDataToJson(range, channelSetting, jsonRange);
        jsonRanges.insert(range, jsonRange);
    }

    jsonAll.insert(noClampJsonId, jsonRanges);

    QJsonDocument doc(jsonAll);
    qInfo("----------------- json range constants generated for %s -----------------", qPrintable(channelSetting->m_sAlias1));
    qInfo("%s", qPrintable(doc.toJson(QJsonDocument::Indented)));
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
