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

    bool isChannelRangeAvailable(const QString &channelName, const QString &rangeName);
    AdjDataRange getRangeAdjData(const QString &channelName, const QString &rangeName);
    void setChannelRange(const QString &channelName, const QString &rangeName, const AdjDataRange &adjData);

private:
    AdjDataHeaderIntern m_adjHeader;
    QMap<QString, QMap<QString, AdjDataRange>> m_rangeInfosMap;
};

#endif // ADJDATACOMPLETEINTERN_H
