#ifndef ADJDATACOMPLETEINTERN_H
#define ADJDATACOMPLETEINTERN_H

#include "adjdataheaderintern.h"
#include "adjdatarangegroup.h"
#include <QMap>

class AdjDataCompleteIntern
{
public:
    bool isEmpty();

    AdjDataHeaderIntern& getAdjHeader();

    bool isChannelRangeAvailable(QString channelName, QString rangeName);
    AdjDataRangeGroup getRangeAdjData(QString channelName, QString rangeName);
    void setChannelRange(QString channelName, QString rangeName, AdjDataRangeGroup adjData);

private:
    AdjDataHeaderIntern m_adjHeader;
    QMap<QString, QMap<QString, AdjDataRangeGroup>> m_rangeInfosMap;
};

#endif // ADJDATACOMPLETEINTERN_H
