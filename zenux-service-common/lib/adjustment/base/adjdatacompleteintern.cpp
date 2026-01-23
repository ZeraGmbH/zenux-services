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

AdjDataRange AdjDataCompleteIntern::getRangeAdjData(QString channelName, QString rangeName)
{
    AdjDataRange adjGroup;
    if(isChannelRangeAvailable(channelName, rangeName))
        adjGroup = m_rangeInfosMap[channelName][rangeName];
    return adjGroup;
}

void AdjDataCompleteIntern::setChannelRange(QString channelName, QString rangeName, AdjDataRange adjData)
{
    m_rangeInfosMap[channelName][rangeName] = adjData;
}
