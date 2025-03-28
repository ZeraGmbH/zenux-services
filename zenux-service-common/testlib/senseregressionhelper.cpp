#include "senseregressionhelper.h"
#include "scpisingletransactionblocked.h"
#include <timemachineobject.h>
#include <QSignalSpy>
#include <QJsonDocument>
#include <QJsonArray>

static const QString JsonNameStr = QStringLiteral("name");
static const QString JsonAliasStr = QStringLiteral("alias");
static const QString JsonAvailStr = QStringLiteral("avail");
static const QString JsonUrValStr = QStringLiteral("urval");
static const QString JsonRejectionStr = QStringLiteral("rejection");
static const QString JsonOvRejectionStr = QStringLiteral("ovrejection");
static const QString JsonAdcRejectionStr = QStringLiteral("adcrejection");
static const QString JsonAdjustStatusFlags = QStringLiteral("adjuststatusflags");

QString SenseRegressionHelper::getJsonNumString(int clampTypeNo)
{
    return "clamp_type_no_" + (QString("0000") + QString("%1").arg(clampTypeNo)).right(4);
}

void SenseRegressionHelper::addRangeConstantDataToJson(QString rangeName, SenseSystem::cChannelSettings *channelSettings, QJsonObject &range)
{
    range.insert(JsonNameStr, rangeName);

    QString channelName = channelSettings->m_nameMx;

    // stolen from cPCBInterfacePrivate
    QString alias = ScpiSingleTransactionBlocked::query(QString("SENS:%1:%2:ALIAS?").arg(channelName, rangeName));
    range.insert(JsonAliasStr, alias);

    QString avail = ScpiSingleTransactionBlocked::query(QString("SENS:%1:%2:AVAIL?").arg(channelName, rangeName));
    range.insert(JsonAvailStr, avail);

    QString urValue = ScpiSingleTransactionBlocked::query(QString("SENS:%1:%2:URVALUE?").arg(channelName, rangeName));
    range.insert(JsonUrValStr, urValue.toDouble());

    QString rejection = ScpiSingleTransactionBlocked::query(QString("SENS:%1:%2:REJECTION?").arg(channelName, rangeName));
    range.insert(JsonRejectionStr, rejection.toDouble());

    QString ovRejection = ScpiSingleTransactionBlocked::query(QString("SENS:%1:%2:OVREJECTION?").arg(channelName, rangeName));
    range.insert(JsonOvRejectionStr, ovRejection.toDouble());

    QString adcRejection = ScpiSingleTransactionBlocked::query(QString("SENS:%1:%2:ADCREJECTION?").arg(channelName, rangeName));
    range.insert(JsonAdcRejectionStr, adcRejection.toDouble());

    QString adjustStatusFlags = ScpiSingleTransactionBlocked::query(QString("SENS:%1:%2:TYPE?").arg(channelName, rangeName));
    range.insert(JsonAdjustStatusFlags, adjustStatusFlags);
}

bool SenseRegressionHelper::compareRangeConstantDataWithJson(QJsonObject &rangeReference, QString clampName, QString rangeName, SenseSystem::cChannelSettings *channelSetting)
{
    QString channelName = channelSetting->m_nameMx;
    bool allOk = !rangeReference.isEmpty();
    if(allOk) {
        QString alias = ScpiSingleTransactionBlocked::query(QString("SENS:%1:%2:ALI?").arg(channelName, rangeName));
        QString expected = rangeReference.value(JsonAliasStr).toString();
        if(alias != expected) {
            reportError(clampName, rangeName, JsonAliasStr, expected, alias);
            allOk = false;
        }

        QString avail = ScpiSingleTransactionBlocked::query(QString("SENS:%1:%2:AVA?").arg(channelName, rangeName));
        expected = rangeReference.value(JsonAvailStr).toString();
        if(avail != expected) {
            reportError(clampName, rangeName, JsonAvailStr, expected, avail);
            allOk = false;
        }

        QString urValue = ScpiSingleTransactionBlocked::query(QString("SENS:%1:%2:URV?").arg(channelName, rangeName));
        expected = rangeReference.value(JsonUrValStr).toString();
        if(urValue != expected) {
            reportError(clampName, rangeName, JsonUrValStr, expected, urValue);
            allOk = false;
        }

        QString rejection = ScpiSingleTransactionBlocked::query(QString("SENS:%1:%2:REJ?").arg(channelName, rangeName));
        expected = rangeReference.value(JsonRejectionStr).toString();
        if(rejection != expected) {
            reportError(clampName, rangeName, JsonRejectionStr, expected, rejection);
            allOk = false;
        }

        QString ovRejection = ScpiSingleTransactionBlocked::query(QString("SENS:%1:%2:OVR?").arg(channelName, rangeName));
        expected = rangeReference.value(JsonOvRejectionStr).toString();
        if(ovRejection != expected) {
            reportError(clampName, rangeName, JsonOvRejectionStr, expected, ovRejection);
            allOk = false;
        }

        QString adcRejection = ScpiSingleTransactionBlocked::query(QString("SENS:%1:%2:ADCR?").arg(channelName, rangeName));
        expected = rangeReference.value(JsonAdcRejectionStr).toString();
        if(adcRejection != expected) {
            reportError(clampName, rangeName, JsonAdcRejectionStr, expected, adcRejection);
            allOk = false;
        }

        QString adjustStatusFlags = ScpiSingleTransactionBlocked::query(QString("SENS:%1:%2:TYPE?").arg(channelName, rangeName));
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

static QString noClampJsonId = QStringLiteral("no-clamps");

QByteArray SenseRegressionHelper::genJsonConstantValuesAllRanges(QList<SenseSystem::cChannelSettings*> channelSettings, Zera::cPCBInterface* pcbIFace)
{
    QJsonObject jsonAll;
    for(const auto &channelSetting : channelSettings) {
        QSignalSpy responseSpy(pcbIFace, &Zera::cPCBInterface::serverAnswer);
        pcbIFace->getRangeList(channelSetting->m_nameMx);
        TimeMachineObject::feedEventLoop();

        QJsonArray jsonRanges;
        const QStringList ranges = responseSpy[0][2].toStringList();
        for(const QString &range : ranges) {
            QJsonObject jsonRange;
            SenseRegressionHelper::addRangeConstantDataToJson(range, channelSetting, jsonRange);
            jsonRanges.append(jsonRange);
        }

        QString channelName = channelSetting->m_nameMx;
        if(!channelSetting->m_sAlias1.isEmpty())
            channelName += "/" + channelSetting->m_sAlias1;
        if(!channelSetting->m_sAlias2.isEmpty())
            channelName += "/" + channelSetting->m_sAlias2;
        jsonAll.insert(channelName, jsonRanges);
    }
    QJsonDocument doc(jsonAll);
    return doc.toJson(QJsonDocument::Indented);
}

void SenseRegressionHelper::reportError(QString clampName, QString range, QString entry, QString expected, QString found)
{
    qCritical("Clamp: \"%s\" / Range: \"%s\" / Entry: \"%s\": Constant incorrect. Expected: \"%s\" / Found: \"%s\"",
              qPrintable(clampName),
              qPrintable(range),
              qPrintable(entry),
              qPrintable(expected),
              qPrintable(found));
}
