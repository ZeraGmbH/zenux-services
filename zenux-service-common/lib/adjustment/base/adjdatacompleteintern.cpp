#include "adjdatacompleteintern.h"

bool AdjDataCompleteIntern::isEmpty()
{
    return m_rangeInfosMap.isEmpty();
}

AdjDataHeaderIntern &AdjDataCompleteIntern::getAdjHeader()
{
    return m_adjHeader;
}

bool AdjDataCompleteIntern::isChannelRangeAvailable(const QString &channelName, const QString &rangeName)
{
    return m_rangeInfosMap.contains(channelName) && m_rangeInfosMap[channelName].contains(rangeName);
}

AdjDataRange AdjDataCompleteIntern::getRangeAdjData(const QString &channelName, const QString &rangeName)
{
    AdjDataRange adjGroup;
    if(isChannelRangeAvailable(channelName, rangeName))
        adjGroup = m_rangeInfosMap[channelName][rangeName];
    return adjGroup;
}

void AdjDataCompleteIntern::setChannelRange(const QString &channelName,
                                            const QString &rangeName,
                                            const AdjDataRange &adjData)
{
    m_rangeInfosMap[channelName][rangeName] = adjData;
}
