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
static const QString JsonAdjustControllerSelection = QStringLiteral("ctrlselection");

QString SenseRegressionHelper::getJsonNumString(int clampTypeNo)
{
    return "clamp_type_no_" + (QString("0000") + QString("%1").arg(clampTypeNo)).right(4);
}

void SenseRegressionHelper::addRangeConstantDataToJson(const QString &rangeName,
                                                       const SenseSystem::cChannelSettings *channelSettings,
                                                       QJsonObject &range)
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

    QString ctrlSelectionNum = ScpiSingleTransactionBlocked::query(QString("SENS:%1:%2:CTRLSELECTION?").arg(channelName, rangeName));
    range.insert(JsonAdjustControllerSelection, ctrlSelectionNum);
}

static QString noClampJsonId = QStringLiteral("no-clamps");

QByteArray SenseRegressionHelper::genJsonConstantValuesAllRanges(const QList<SenseSystem::cChannelSettings *> &channelSettings,
                                                                 Zera::cPCBInterface* pcbIFace)
{
    QJsonObject jsonAll;
    for(const auto &channelSetting : channelSettings) {
        QSignalSpy responseSpy(pcbIFace, &AbstractServerInterface::serverAnswer);
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

void SenseRegressionHelper::reportError(const QString &clampName, const QString &range, const QString &entry, const QString &expected, const QString &found)
{
    qCritical("Clamp: \"%s\" / Range: \"%s\" / Entry: \"%s\": Constant incorrect. Expected: \"%s\" / Found: \"%s\"",
              qPrintable(clampName),
              qPrintable(range),
              qPrintable(entry),
              qPrintable(expected),
              qPrintable(found));
}
