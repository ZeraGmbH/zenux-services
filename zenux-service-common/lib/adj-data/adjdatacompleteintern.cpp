#include "adjdatacompleteintern.h"

bool AdjDataCompleteIntern::isEmpty()
{
    return m_rangeInfosMap.isEmpty();
}

AdjDataHeaderIntern &AdjDataCompleteIntern::getAdjHeader()
{
    return m_adjHeader;
}

bool AdjDataCompleteIntern::isChannelRangeAvailable(QString channelName, QString rangeName)
{
    return m_rangeInfosMap.contains(channelName) && m_rangeInfosMap[channelName].contains(rangeName);
}

AdjDataRangeGroup AdjDataCompleteIntern::getRangeAdjData(QString channelName, QString rangeName)
{
    AdjDataRangeGroup adjGroup;
    if(isChannelRangeAvailable(channelName, rangeName))
        adjGroup = m_rangeInfosMap[channelName][rangeName];
    return adjGroup;
}

void AdjDataCompleteIntern::setChannelRange(QString channelName, QString rangeName, AdjDataRangeGroup adjData)
{
    m_rangeInfosMap[channelName][rangeName] = adjData;
}

QMap<QString, QStringList> AdjDataCompleteIntern::getRangeInfos()
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
