#ifndef ADJUSTMENTDATASET_H
#define ADJUSTMENTDATASET_H

#include "adjustmendataheader.h"
#include "adjustmentdatarangegroup.h"
#include <QMap>
#include <memory>

class AdjustmentDataSet
{
public:
    AdjustmentDataHeader& getAdjHeader();
    bool isChannelRangeAvailable(QString channelName, QString rangeName);
    std::shared_ptr<AdjustmentDataRangeGroup> getRangeAdjData(QString channelName, QString rangeName);
    void setChannelRange(QString channelName, QString rangeName, std::shared_ptr<AdjustmentDataRangeGroup> adjData);
    QMap<QString, QStringList> getRangeInfos();

private:
    AdjustmentDataHeader m_adjHeader;
    QMap<QString, QMap<QString, std::shared_ptr<AdjustmentDataRangeGroup>>> m_rangeInfosMap;
};

#endif // ADJUSTMENTDATASET_H
