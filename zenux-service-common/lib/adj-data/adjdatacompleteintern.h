#ifndef ADJDATACOMPLETEINTERN_H
#define ADJDATACOMPLETEINTERN_H

#include "adjdataheaderintern.h"
#include "adjdatarangegroup.h"
#include <QMap>
#include <memory>

class AdjDataCompleteIntern
{
public:
    AdjDataHeaderIntern& getAdjHeader();
    bool isChannelRangeAvailable(QString channelName, QString rangeName);
    std::shared_ptr<AdjDataRangeGroup> getRangeAdjData(QString channelName, QString rangeName);
    void setChannelRange(QString channelName, QString rangeName, std::shared_ptr<AdjDataRangeGroup> adjData);
    QMap<QString, QStringList> getRangeInfos();

private:
    AdjDataHeaderIntern m_adjHeader;
    QMap<QString, QMap<QString, std::shared_ptr<AdjDataRangeGroup>>> m_rangeInfosMap;
};

#endif // ADJDATACOMPLETEINTERN_H
