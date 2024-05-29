#ifndef ADJDATACOMPLETEINTERN_H
#define ADJDATACOMPLETEINTERN_H

#include "adjdataheaderintern.h"
#include "adjdatarange.h"
#include <QMap>

class AdjDataCompleteIntern
{
public:
    bool isEmpty();

    AdjDataHeaderIntern& getAdjHeader();

    bool isChannelRangeAvailable(QString channelName, QString rangeName);
    AdjDataRange getRangeAdjData(QString channelName, QString rangeName);
    void setChannelRange(QString channelName, QString rangeName, AdjDataRange adjData);

private:
    AdjDataHeaderIntern m_adjHeader;
    QMap<QString, QMap<QString, AdjDataRange>> m_rangeInfosMap;
};

#endif // ADJDATACOMPLETEINTERN_H
