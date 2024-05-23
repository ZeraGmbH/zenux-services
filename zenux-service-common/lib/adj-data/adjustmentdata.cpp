#include "adjustmentdata.h"

AdjustmentDataHeader &AdjustmentData::getAdjHeader()
{
    return m_adjHeader;
}

bool AdjustmentData::isChannelRangeAvailable(QString channelName, QString rangeName)
{
    return m_rangeInfosMap.contains(channelName) && m_rangeInfosMap[channelName].contains(rangeName);
}

void AdjustmentData::setChannelRange(QString channelName, QString rangeName, std::shared_ptr<AdjustmentRangeData> adjData)
{
    m_rangeInfosMap[channelName][rangeName] = adjData;
}

QMap<QString, QStringList> AdjustmentData::getRangeInfos()
{
    QMap<QString, QStringList> info;
    for(auto channelIter = m_rangeInfosMap.cbegin(); channelIter!=m_rangeInfosMap.cend(); ++channelIter) {
        QString channelName = channelIter.key();
        const auto& rangeData = channelIter.value();
        for(auto rangeIter=rangeData.cbegin(); rangeIter!=rangeData.constEnd(); ++rangeIter)
            info[channelName].append(rangeIter.key());
    }
    return info;
}
