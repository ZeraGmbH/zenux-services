#ifndef ADJDATACOMPLETEINTERN_H
#define ADJDATACOMPLETEINTERN_H

#include "adjdataheaderintern.h"
#include "adjdatarangegroup.h"
#include <QMap>

class AdjDataCompleteIntern
{
public:
    AdjDataHeaderIntern& getAdjHeader();
    bool isChannelRangeAvailable(QString channelName, QString rangeName);
    AdjDataRangeGroup getRangeAdjData(QString channelName, QString rangeName);
    void setChannelRange(QString channelName, QString rangeName, AdjDataRangeGroup adjData);
    QMap<QString, QStringList> getRangeInfos();

private:
    AdjDataHeaderIntern m_adjHeader;
    QMap<QString, QMap<QString, AdjDataRangeGroup>> m_rangeInfosMap;
};

#endif // ADJDATACOMPLETEINTERN_H
